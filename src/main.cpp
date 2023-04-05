#include <Arduino.h>
#include "global.h"
#include "EasyNextionLibrary.h"
#include "StepperMotor.h"
#include <max6675.h>
#include <EEPROM.h>
#include "RTClib.h"
#include <AutoPID.h>
#include <ESPUI.h>
#include "SPIFFS.h"
bool blockHeatDn = false;

void buttonOn(Control* sender, int type){
    if(globalState.mode == globalState.idle){ //розжиг
        globalState.mode = globalState.kindling;
    }
}
void buttonOff(Control *sender, int type){
    if(globalState.mode == globalState.run){
        globalState.mode = globalState.stop;
    }
}
void slicerCLB(Control *sender, int type){
  static int preValue = -100;
    if(globalState.mode == globalState.run){
      if(sender->value.toInt() > 0 && sender->value.toInt() < 100){
        globalState.power = sender->value.toInt();
        if(preValue > globalState.power) blockHeatDn = true;
      }
    }
  preValue = globalState.power;
}

struct{
  bool out;
  uint16_t period = 5; 
  double val;
}heatStr;
SoftPwm heat(&heatStr.out, &heatStr.period, &heatStr.val);




RTC_DS1307 rtc;
struct{
    double val;
    double set;
    bool   out;
}pid;

AutoPIDRelay myPid(&pid.val, &pid.set, &pid.out, 
1 * 1000,    /* time*/
0.005,  /* kp*/
0.0000,  /* ki*/
0 * 0.001); /* kd*/
MAX6675 thermBurner(27, 26, 25);// MAX6675 thermBurner(26, 27, 13);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// #include "ESP32TimerInterrupt.h"
// ESP32Timer ITimer0(0);
const int pinDir    = 14; //4;
const int pinStep   = 2;
const int pinEnable = 5; //33;

StepperMotor pump(ledcSetup, ledcAttachPin, ledcWrite, ledcWriteTone, pinStep, 0, 8, 500, pinDir);
void initSetting(void);
float frequency = 10;
const bool debug = false;

const int pinHeater = 23; //19; // нагреватель  горелки
const int pinCandle = 13; //23; // свеча горелки
const int pinValve  = 4; //32; // клапан

bool beep; 

const int pinFullTank   = 34; // 36; // датчик полного бака 
const int pinLeak       = 32; //18; // датчик протечки
const int pinBeep       = 19; //25; // пищалка
//const int pinFan        = 15; // вентилятор охлаждения
const int pinLevel      = 35; // уровень в горелке

const int pinTempBurner = 36; // температура горелки sp
const int pinTempTank   = 39; // температура бака sn
const int pinLevelFault = 15; // датчик перелива через горелку
DateTime currDt;
structSettingName settingName;
structSetting globalSetting[settingName.count];
EasyNex lcd(Serial2); // 17tx  16rx выводы на есп
structTimer dataTimer;
structLanguage dataLanguage;
structGlobalState globalState;




void setup() {
  EEPROM.begin(2048);
  ESPUI.setVerbosity(Verbosity::VerboseJSON);
  Serial.begin(115200);
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  WiFi.mode(WIFI_AP);
  IPAddress apIP(192, 168, 0, 10);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Fireplace", "87654321");


  myPid.setBangBang(100);
  myPid.setTimeStep(1000);
  rtc.begin();
  
  //analogSetAttenuation(ADC_2_5db);
  pinMode(pinTempBurner, ANALOG);
  pinMode(pinLevel, ANALOG);
  pinMode(pinTempTank, ANALOG);
  //pinMode(pinFan, OUTPUT);
  pinMode(pinBeep, OUTPUT);
  digitalWrite(pinBeep, LOW);
  pinMode(pinFullTank, INPUT);
  pinMode(pinLeak, INPUT);
  pinMode(pinLevelFault, INPUT);
  globalState.currentTemperatureBurner = 30;
  globalState.power = 50;
  globalSetting[settingName.timeRefuel].val = 30;
  globalSetting[settingName.stepsMl].val = 1000;
  globalSetting[settingName.speedRefuel].val = 1000;

  // ledcSetup(0, 10, 8);
  // ledcAttachPin(2, 0);
  // ledcWrite(0, 50);
  // ledcWriteTone(0, 200);

  // ITimer0.setFrequency(1000, TimerHandler0);
  // ITimer0.attachInterruptInterval(frequensy, TimerHandler0);
  // ITimer0.stopTimer();
  //pinMode(pinStep, OUTPUT);
  pinMode(pinDir, OUTPUT);
  pinMode(pinEnable, OUTPUT);
  //digitalWrite(pinStep, LOW);
  digitalWrite(pinDir, LOW);
  digitalWrite(pinEnable, HIGH);
  pinMode(pinHeater, OUTPUT);
  pinMode(pinValve, OUTPUT);
  pinMode(pinCandle, OUTPUT);
  valveOnOff(false);
  candleOnOff(false);
  globalState.mode = globalState.idle;
  dataLanguage.lang = dataLanguage.ua;
  dataTimer.onHour = 5;
  dataTimer.onMinure = 58;
  dataTimer.offTimer = 2;
  
  lcd.begin(115200);
  initSetting();

  lcd.writeStr("page main");
  pump.pulseMl =  globalSetting[settingName.stepsMl].val;

  EEPROM.get(1200, pump.counter);
  EEPROM.get(500, dataTimer);
  globalState.onSound = EEPROM.read(1100) == 1 ? true : false;
  analogSetAttenuation(ADC_11db);

  // ESPUI.slider("Slider one", &sliderCLB, ControlColor::Alizarin, 30, 0, 100);
  ESPUI.button("On:",   buttonOn, Turquoise, "Start");
  ESPUI.slider("Level", slicerCLB, ControlColor::Turquoise, 23, 0, 100);
  ESPUI.button("Off",   buttonOff, ControlColor::Turquoise, "Stop");

  

  ESPUI.begin("Fireplace");
}

void loop() {
  heat.run(millis());
  myPid.run();
  lcd.NextionListen();
  prckindling();
  prcRun();
  prcStop();
  prcRefueling();
  prcError();
  // ITimer0.setFrequency(frequency, TimerHandler0);
  pump.run();
  {
    static unsigned long t = millis();
    if(millis() - t >= 1000UL){
      t = millis();
      //Serial.println(pump.counter);
    }
  }


  {
    static uint32_t t = millis();
    if(blockHeatDn){
      if(millis() - t >= (globalSetting[settingName.blockHeatTime].val * 1000UL)){
        blockHeatDn = false;
      }
    }
    else{
      t = millis();
    }
  }
  // реализовать шим нагрев на горелку

  if(globalState.heater == true && blockHeatDn == false){
    // globalSetting[settingName.tempBurner].val = 90;
    // pid.set = globalSetting[settingName.tempBurner].val;
    // pid.val = globalState.currentTemperatureBurner;
    if(globalState.mode == globalState.kindling){
      heatStr.val = globalSetting[settingName.heatMax].val;
    }
    else{
      heatStr.val = map(globalState.power, 0, 100, globalSetting[settingName.heatMin].val, globalSetting[settingName.heatMax].val);
    }
    


    if(heatStr.out){
      heatOnOff(true);
    }
    else{
      heatOnOff(false);
    }
    // if(globalState.currentTemperatureBurner <  globalSetting[settingName.tempBurner].val ){
    //   heatOnOff(true);
    // }
    // else if(globalState.currentTemperatureBurner >  globalSetting[settingName.tempBurner].val){
    //   heatOnOff(false);
    // }
  }
  else{
    heatOnOff(false);
  }


  {
    static unsigned long t = millis();
    static long integrator = 0;
    static int counter = 0;
    if(millis() - t >= 500UL){
      t = millis();
      if(counter < 10){
        counter++;
        integrator += (int)thermBurner.readCelsius(); 
      }
      else{
        globalState.flameTemperature = integrator / counter;
        if(1)Serial.println("Flame  :" + String(globalState.flameTemperature));
        if(1)Serial.println("tem bur:" + String(globalState.currentTemperatureBurner));
        if(1)Serial.println("tem tan:" + String(globalState.currentTemperatureTank));
        if(1)Serial.println("_________________");


        counter = 0;
        integrator = 0;
      }
    }
  }

  { // температура горелки
    static uint16_t count = 0;
    static unsigned long summ = 0;
    static unsigned long t = millis();
    if(millis() - t >= 200){
      t = millis();
      if(count < 10){
        count++;
        summ += (analogRead(pinTempBurner));
      }
      else{
        count = 0;
        summ = summ / 10;
        globalState.currentTemperatureBurner = Thermistor(summ); //constrain((int)((summ * (1600.0 / 4096.0)) / 10.0), 5, 110);
        if(debug)Serial.println("currentTemperatureBurner :" + String(globalState.currentTemperatureBurner));
        summ = 0;
      }
    }
  }


  { // температура бака
    static uint16_t count = 0;
    static unsigned long summ = 0;
    static unsigned long t = millis();
    if(millis() - t >= 200){
      t = millis();
      if(count < 10){
        count++;
        summ += (analogRead(pinTempTank));
      }
      else{
        count = 0;
        summ = summ / 10;
        globalState.currentTemperatureTank = Thermistor(summ); // constrain((int)((summ * (1600.0 / 4096.0)) / 10.0), 5, 110);
        if(debug)Serial.println("currentTemperatureTank :" + String(globalState.currentTemperatureTank));
        summ = 0;
      }
    }
  }


  { // Датчик уровня в горелке // раньше была температура платы
    static uint16_t count = 0;
    static unsigned long summ = 0;
    static unsigned long t = millis();
    static int out = 0;
    if(millis() - t >= 200){
      t = millis();
      if(count < 5){
        count++;
        summ += (analogRead(pinLevel));
      }
      else{
        count = 0;
        summ = summ / 5;
        out = constrain((int)((summ * (1600.0 / 4096.0)) / 10.0), 5, 110);
        // Serial.println("Sensor t: " + String(summ));
////////////////>
        if(summ < 2000){ // Motor off
          Serial.println("off");
          enMotor = false;
        }
        else { //motor on
          Serial.println("on");
          enMotor = true;
        }



        summ = 0;

        // if(out > globalSetting[settingName.tempOnFan].val){
        //   digitalWrite(pinFan, LOW);
        // }
        // else if(out < globalSetting[settingName.tempOffFan].val)
        // {
        //   digitalWrite(pinFan, HIGH);
        // }
        

      }
    }
  }







  {
    static int preLevel  = -1;
    if(globalState.mode == globalState.run || globalState.mode == globalState.kindling || globalState.mode == globalState.stop || globalState.mode == globalState.refueling){
      if(preLevel != (map(pump.counter, 0, globalSetting[settingName.tankCapasity].val, 0, 100))){
        preLevel = (map(pump.counter, 0, globalSetting[settingName.tankCapasity].val, 0, 100));
        lcd.writeStr("main.t0.txt", String(constrain( (int)(preLevel) + 1, 0, 100) ) + "%" );
      }
    }
  }




  { // beep
    static unsigned long t = millis();
    if(globalState.onSound){
      if(beep){
        beep = false;
        t = millis();
        digitalWrite(pinBeep, HIGH);
      }
      if(millis() - t >= 300UL && digitalRead(pinBeep) == HIGH){
        digitalWrite(pinBeep, LOW);
      }
    }
    else{
        digitalWrite(pinBeep, LOW);
    }

  }


  {
    static unsigned long t = millis();
    if(millis() - t >= 1000UL){
      t = millis();
    
        DateTime now = rtc.now();
        currDt = now;
        // Serial.print(now.year(), DEC);
        // Serial.print('/');
        // Serial.print(now.month(), DEC);
        // Serial.print('/');
        // Serial.print(now.day(), DEC);
        // Serial.print(" (");
        // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
        // Serial.print(") ");
        // Serial.print(now.hour(), DEC);
        // Serial.print(':');
        // Serial.print(now.minute(), DEC);
        // Serial.print(':');
        // Serial.print(now.second(), DEC);
        // Serial.println();
    }
  }


  {
    static unsigned long t = millis();
    static bool timerOn = false;
      int dOfWeek;
        if(currDt.dayOfTheWeek() == 0) {
          dOfWeek = 6;
        }
        else{
          dOfWeek = currDt.dayOfTheWeek() - 1;
        }    
    if(globalState.mode == globalState.idle){
      if(dataTimer.day[dOfWeek]){
        if(dataTimer.onHour == currDt.hour()){
          if(dataTimer.onMinure == currDt.minute()){
            globalState.mode = globalState.kindling;
            timerOn = true;
            t = millis();
          }
        }
      }
    }
    if(timerOn && millis() - t >= (unsigned long)dataTimer.offTimer * 1000UL * 60UL * 60UL){
      timerOn = false;
      if(globalState.mode == globalState.run){
          globalState.mode = globalState.stop;
      }
    }
  }


}

void initSetting(void){
  globalSetting[settingName.tankCapasity] = {
    10, 100000, 1000, {"Емкость бака", "мл"}
  };
  globalSetting[settingName.speedRefuel] = {
    1, 1000, 10, {"Скорость заправки", "мл/мин"}
  };
  globalSetting[settingName.timeRefuel] = {
   1, 10000, 60,{"Время заправки", "сек"} 
  };
  globalSetting[settingName.stepsMl] = {
    1, 100000UL, 23650,{"Количество шагов", "на 1 мл"}
  };
  globalSetting[settingName.minPumpSpeed] = {
    1, 10000, 1,{"Мин скорость", "мл/мин"}
  };
  globalSetting[settingName.maxPumpSpeed] = {
    1, 10000, 5,{"Мак скорость", "мл/мин"}
  };
  globalSetting[settingName.tempBurner] = {
    10, 100, 80,{"Температура горелки", "поддерживаемая"}
  };
  globalSetting[settingName.maxTempBurner] = {
    10, 110, 110,{"Температура горелки", "аварийная"}
  };
  globalSetting[settingName.tempFlameSens] = {
    10, 300, 100,{"Температура наличия", "пламени"}
  };
  // globalSetting[settingName.countFlameSens] = {
  //   10, 300, 120,{"Фильтр датчика пл.", "кол измерений"}
  // };
  globalSetting[settingName.maxTempTank] = {
    10, 100, 80,{"Макс температура", "бачка"}
  };
  // globalSetting[settingName.adcRefuelLevel] = {
  //   10, 4000, 120,{"Датчик полного бака", "значение ацп:"}
  // };
  // globalSetting[settingName.adcLeakSensor] = {
  //   10, 4000, 120,{"Датчик протечки", "значение ацп:"}
  // };
  globalSetting[settingName.timeOnCandle] = {
    1, 100, 25,{"Время работы", "свечи"}
  };
  globalSetting[settingName.tempOffFan] = {
    10, 100, 27,{"Т. выключения", "вентилятора"}
  };
  globalSetting[settingName.kindlingTimePump] = {
    1, 10000, 1,{"Розжиг время вкл ", "насоса"}
  };
  globalSetting[settingName.kindlingPowerPump] = {
    1, 10000, 1,{"Розжиг скорость ", "насоса мл/мин"}
  };
  // globalSetting[settingName.kindlingVolumePump] = {
  //   1, 10000, 120,{"Розжиг обЪем закачки", "насоса мл"}
  // };
  globalSetting[settingName.kindlingTimeCandle] = {
    1, 10000, 2,{"Розжиг время вкл", "свечи сек"}
  };
  globalSetting[settingName.kindlingTimeEnd] = {
    1, 10000, 30,{"Розжиг время", "режим работа"}
  };
  globalSetting[settingName.stopPowerPump] = {
    1, 1000, 1,{"Стоп скорость", "насоса мл/мин"}
  };
  globalSetting[settingName.stopTimePump] = {
    1, 1000, 12,{"Стоп время откачки", "насоса сек"}
  };
  globalSetting[settingName.heatMin] = {
    0, 100, 12,{"Мин мощность", "нагревателя"}
  };
  globalSetting[settingName.heatMax] = {
    0, 100, 75,{"Макс мощность", "нагревателя"}
  };
  globalSetting[settingName.blockHeatTime] = {
    1, 100, 5,{"Время блок.нагрева", "сек"}
  };


  if(EEPROM.read(1000) != 7){
    EEPROM.write(1000, 7);
    EEPROM.commit();
    for(int i = 0; i < settingName.count - 2; i++){
      EEPROM.put(i * 10, globalSetting[i].val);
      EEPROM.commit();
      structTimer temp;
      temp.offTimer = 2;
      temp.onHour = 12;
      temp.onMinure = 7;
      EEPROM.put(500, temp);
      EEPROM.commit();      
      // long t = 0;
      // EEPROM.get(i * 10, t);
      // Serial.println("Adr :"+ String(i * 10) + "Val : " + String(t));
    }

    EEPROM.put(1200, pump.counter);
    EEPROM.commit();  
    EEPROM.put(500, dataTimer);
    EEPROM.commit();  

  }
  for(int i = 0; i < settingName.count - 2; i++){
    EEPROM.get(i * 10, globalSetting[i].val);
    // long t = 0;
    // EEPROM.get(i * 10, t);
    // Serial.println("Adr :"+ String(i * 10) + "Val : " + String(t));
  }
}


void valveOnOff(bool onOff){
  if(onOff){
    digitalWrite(pinDir, LOW);
    digitalWrite(pinValve, HIGH);
  }
  else{
    digitalWrite(pinDir, HIGH);
    digitalWrite(pinValve, LOW);
  }
}

void heatOnOff(bool onOff){
  if(onOff){
    digitalWrite(pinHeater, HIGH);
  }
  else{
    digitalWrite(pinHeater, LOW);
  }
}

void candleOnOff(bool onOff){
  if(onOff){
    digitalWrite(pinCandle, HIGH);
  }
  else{
    digitalWrite(pinCandle, LOW);
  }
}

unsigned long  mlMiToFreq(long mlMi){
  // милилитры умножаем на количество импульсов и делим на 60
  return (mlMi * globalSetting[settingName.stepsMl].val) / 60;
}



double Thermistor(int RawADC) {
  double adcMax, Vs;

  double R1 = 47000;//9980.0;   // voltage divider resistor value
  double Beta = 3950.0;  // Beta value
  double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
  double Ro = 100000.0;   // Resistance of Thermistor at 25 degree Celsius
  double Vout, Rt = 0;
  double T, Tc, Tf = 0;
  double adc = 0;
  adcMax = 4096; // ADC resolution 12-bit (0-4095)
  Vs = 3.42;        // supply voltage
  adc = RawADC;
  if(adc < 240) adc = 240;
  Vout =  (( (adc - 240)  / 1288) + 0.3);
  Vs = 3.305;        // supply voltage
  Rt = R1 * Vout / (Vs - Vout);
  T = 1/(1/To + log(Rt/Ro)/Beta);    // Temperature in Kelvin
  Tc = T - 273.15;                   // Celsius
  double K = 1;
  double B = 0;
  return (Tc * K) + B;
}



