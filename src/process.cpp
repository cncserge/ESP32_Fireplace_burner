#include "global.h"
#include "EEPROM.h"
bool enMotor = false;
void prckindling(void){ // розжиг
    static int val = 0;
    static bool enReadSensorFire = false; // флаг начала опроса датчика пламени
    static bool enMotorOnOff = false; // флаг бокировки двигателя по датчику
    static bool offCandle = false;
    static int state = 0;
    static unsigned long t[4];
    static unsigned long timCandle = 0;
    if(globalState.mode == globalState.kindling){
        if(state == 0){
            enReadSensorFire = false;
            enMotorOnOff = false;
            offCandle = false;
            lcd.writeNum("main.j0.bpic", 28); /// прогресс
            lcd.writeNum("main.j0.ppic", 29); /// прогресс
            val = 0;
            if(debug)Serial.println("mode kindling start");
            lcd.writeNum("main.p0.pic", 24); /// кнопка
            // blink = false;
            t[0] = t[1] = t[2] = t[3]= millis(); 
            globalState.heater = true;
            state = 1;
        }
        if(state == 1){
            
            if(1){ // изменить на true if(enReadSensorFire){
                if(globalState.flameTemperature > globalSetting[settingName.tempFlameSens].val){
                    candleOnOff(false);
                    globalState.mode = globalState.run;
                }
            }
            if(millis() - t[0] > ( globalSetting[settingName.kindlingTimePump].val * 1000UL ) ){   // через 2 мин включился насос на определенной скорости
                enMotorOnOff = true;
                t[0] = millis() + 10000000UL;
                valveOnOff(false);
                digitalWrite(pinEnable, LOW);
                // pump.start(200, 1, mlMiToFreq(globalSetting[settingName.kindlingPowerPump].val));
                pump.setFrequency(mlMiToFreq(globalSetting[settingName.kindlingPowerPump].val));
            }
            if( (offCandle == false) && millis() - t[1] > (globalSetting[settingName.kindlingTimeCandle].val * 1000UL)){  // через 3 мин включается свеча накала
                enReadSensorFire = true;
                t[1] = millis() + 10000000UL;
                candleOnOff(true);
                timCandle = millis();
                offCandle = true;
            }
            if(offCandle == true){
                if(millis() - timCandle >= (globalSetting[settingName.timeOnCandle].val * 1000UL)){
                    timCandle = millis() + 10000000UL;
                    candleOnOff(false);
                }
            }

            if(millis() - t[2] > (globalSetting[settingName.kindlingTimeEnd].val * 1000UL)){  // через 5 мин переход в режим работа и выключение свечи
                //candleOnOff(false);
                state = 0;
                offCandle = false;
                if(globalState.flameTemperature > globalSetting[settingName.tempFlameSens].val)
                    globalState.mode = globalState.run;
                else{ // ошибка пламени
                        enMotorOnOff = false;
                        enReadSensorFire = false;
                        globalState.mode = globalState.error;
                        pump.stop();
                        digitalWrite(pinEnable, HIGH);
                        heatOnOff(false);
                        valveOnOff(false);
                        candleOnOff(false);
                        globalState.heater = false;
                        lcd.writeStr("page error");
                        if(dataLanguage.lang == dataLanguage.ua) {
                            lcd.writeStr("error.t0.txt", "Нема");   
                            lcd.writeStr("error.t1.txt", "вогню");  
                            lcd.writeStr("error.t2.txt", "");}
                        if(dataLanguage.lang == dataLanguage.ru) {
                            lcd.writeStr("error.t0.txt", "Отсутствие");   
                            lcd.writeStr("error.t1.txt", "пламени"); 
                            lcd.writeStr("error.t2.txt", "");}
                        if(dataLanguage.lang == dataLanguage.en) {
                            lcd.writeStr("error.t0.txt", "No");   
                            lcd.writeStr("error.t1.txt", "fire");
                            lcd.writeStr("error.t2.txt", "");}
                }
            }
        }
        if(enMotorOnOff){
            if(enMotor){
                pump.setFrequency(mlMiToFreq(globalSetting[settingName.kindlingPowerPump].val));
            }
            else{
                pump.stop();
            }
        }
    }
    else{
        offCandle = false;
        state = 0;
    }

    if(state != 0){
        if(millis() - t[3] >= 250UL){
            t[3] = millis();
            if(val <= 100){
                
                lcd.writeNum("main.j0.val", val); /// прогресс
                val += 10;
            }
            else{
                val = 0;
            }
            // if(blink){
            //     lcd.writeNum("main.j0.bpic", 28); /// прогресс
            //     lcd.writeNum("main.j0.ppic", 29); /// прогресс
            //     lcd.writeNum("main.j0.val", 0); /// прогресс
            //     blink = false;
            // }
            // else{
            //     lcd.writeNum("main.j0.bpic", 28); /// прогресс
            //     lcd.writeNum("main.j0.ppic", 29); /// прогресс
            //     lcd.writeNum("main.j0.val", 100); /// прогресс
            //     blink = true;
            // }
        }
    }

}
// включился тен
// через 2 мин включился насос на определенной скорости
// через 3 мин включается свеча накала
// через 5 мин переход в режим работа и выключение свечи


void prcRun(void){ //  горение
    static unsigned long t_off_candle = millis();
    static int state = 0;
    static int prePower = -100;
    if(globalState.mode == globalState.run){
        if(state == 0){
            if(debug)Serial.println("mode run start");
            state = 1;
            lcd.writeNum("main.p2.pic", 25); /// up
            lcd.writeNum("main.p1.pic", 26); /// dn
            // lcd.writeNum("main.j0.val", globalState.power); /// прогресс
            // prePower = globalState.power;
            t_off_candle = millis();
        }
        if(state == 1){
            if(prePower != globalState.power){
                prePower = globalState.power;
                lcd.writeNum("main.j0.val", globalState.power);
                if(debug)Serial.println("mode run start state 1");
                if(debug)Serial.println(globalState.power);
                if(debug)Serial.println(map(globalState.power, 0, 100, globalSetting[settingName.minPumpSpeed].val, globalSetting[settingName.maxPumpSpeed].val));
                if(debug)Serial.println(mlMiToFreq(map(globalState.power, 0, 100, globalSetting[settingName.minPumpSpeed].val, globalSetting[settingName.maxPumpSpeed].val)));

            }
            if(digitalRead(pinCandle) == HIGH && millis() - t_off_candle >= 15000UL){
                candleOnOff(false);
            }
            if(enMotor)
                pump.setFrequency( mlMiToFreq( map(globalState.power, 0, 100, globalSetting[settingName.minPumpSpeed].val, globalSetting[settingName.maxPumpSpeed].val) ) );
            else
                pump.stop();
        }
    }
    else{
        t_off_candle = millis();
        state = 0;
        prePower = -100;
    }
}

void prcStop(void){ // остановка
    static int state = 0;
    static int  val = 100;
    static unsigned long t[2];
    if(globalState.mode == globalState.stop){
        if(state == 0){ // включаем насос в реверс на определенное время
            globalState.heater = false;
            digitalWrite(pinDir, LOW);
            pump.setFrequency( mlMiToFreq( globalSetting[settingName.stopPowerPump].val) );
            val = 100;
            if(debug)Serial.println("mode stop start");
            t[1] = t[0] = millis();
            state = 1;
            lcd.writeNum("main.p2.pic", 27); /// up
            lcd.writeNum("main.p1.pic", 27); /// dn
            candleOnOff(false);
        }
        if(state == 1){
            if(millis() - t[0] >= globalSetting[settingName.stopTimePump].val * 1000UL){
                pump.stop();
                digitalWrite(pinEnable, HIGH);
                state = 2;
            }
        }
        if(state == 2 && globalState.flameTemperature < globalSetting[settingName.tempFlameSens].val){/// опрашиваем датчик пламени как пламя исчезло котел погас
            globalState.mode = globalState.idle;
            lcd.writeNum("main.p0.pic", 3); /// кнопка
            lcd.writeNum("main.j0.bpic", 30); /// прогресс
            lcd.writeNum("main.j0.ppic", 30); /// прогресс
            state = 0;
            EEPROM.put(1200, pump.counter);
            EEPROM.commit();
        }
    }
    else{
        state = 0;
    }

    if(state != 0){
        if(millis() - t[1] >= 250UL){
            t[1] = millis();
            if(val >= 0){
                
                lcd.writeNum("main.j0.val", val); /// прогресс
                val -= 10;
            }
            else{
                val = 100;
            }
        }
    }

}

void prcError(void){
    static int stateFullSensor = 0;

    if(globalState.mode != globalState.error){
        stateFullSensor = 0;
    }

    {
        static int state = 0;
        static int val = 100;
        static unsigned long tim = millis();
        if(stateFullSensor == 1){
            if(state == 0){ // включаем насос в реверс на определенное время
                globalState.heater = false;
                digitalWrite(pinEnable, LOW);
                digitalWrite(pinDir, LOW);
                pump.setFrequency( mlMiToFreq( globalSetting[settingName.stopPowerPump].val) );
                val = 100;
                state = 1;
                candleOnOff(false);
                tim = millis();
            }
            if(state == 1){
                if(millis() - tim >= globalSetting[settingName.stopTimePump].val * 1000UL){
                    pump.stop();
                    digitalWrite(pinEnable, HIGH);
                    state = 2;
                    globalState.mode = globalState.error;
                    stateFullSensor = 0;
                }
            }
        }
        else{
            state = 0;
        }
    }




/////////////////////////////////////HIGH
    if(digitalRead(pinLevelFault) == LOW && globalState.mode != globalState.error){ // ошибка датчик уровня в горелке
        stateFullSensor = 1;
        globalState.mode = globalState.error;
        pump.stop();
        digitalWrite(pinEnable, HIGH);
        heatOnOff(false);
        valveOnOff(false);
        candleOnOff(false);
        globalState.heater = false;
        lcd.writeStr("page error");
        if(dataLanguage.lang == dataLanguage.ua) {
            lcd.writeStr("error.t0.txt", "Аварiйний");   
            lcd.writeStr("error.t1.txt", "рiвень");  
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.ru) {
            lcd.writeStr("error.t0.txt", "Аварийный");   
            lcd.writeStr("error.t1.txt", "уровень"); 
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.en) {
            lcd.writeStr("error.t0.txt", "Emergency");   
            lcd.writeStr("error.t1.txt", "level");
            lcd.writeStr("error.t2.txt", "");}
    }





    if(digitalRead(pinLeak) == LOW && globalState.mode != globalState.error){ // ошибка датчик прорыва бака
        globalState.mode = globalState.error;
        pump.stop();
        digitalWrite(pinEnable, HIGH);
        heatOnOff(false);
        valveOnOff(false);
        candleOnOff(false);
        globalState.heater = false;
        lcd.writeStr("page error");
        if(dataLanguage.lang == dataLanguage.ua) {
            lcd.writeStr("error.t0.txt", "Розрив");   
            lcd.writeStr("error.t1.txt", "трубки");  
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.ru) {
            lcd.writeStr("error.t0.txt", "Разрыв");   
            lcd.writeStr("error.t1.txt", "трубки"); 
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.en) {
            lcd.writeStr("error.t0.txt", "Tube");   
            lcd.writeStr("error.t1.txt", "rupture");
            lcd.writeStr("error.t2.txt", "");}
    }

    if(globalState.mode != globalState.error 
        && globalState.mode == globalState.run 
        && globalState.flameTemperature < globalSetting[settingName.tempFlameSens].val){ // ошибка датчик пламени

        globalState.mode = globalState.error;
        pump.stop();
        digitalWrite(pinEnable, HIGH);
        heatOnOff(false);
        valveOnOff(false);
        candleOnOff(false);
        globalState.heater = false;
        lcd.writeStr("page error");
        if(dataLanguage.lang == dataLanguage.ua) {
            lcd.writeStr("error.t0.txt", "Нема");   
            lcd.writeStr("error.t1.txt", "вогню");  
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.ru) {
            lcd.writeStr("error.t0.txt", "Отсутствие");   
            lcd.writeStr("error.t1.txt", "пламени"); 
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.en) {
            lcd.writeStr("error.t0.txt", "No");   
            lcd.writeStr("error.t1.txt", "fire");
            lcd.writeStr("error.t2.txt", "");}
    }

    if(    globalState.mode != globalState.error 
        && globalState.mode != globalState.idle 
        && globalState.currentTemperatureTank > globalSetting[settingName.maxTempTank].val){ // ошибка датчик температуры бачка

        globalState.mode = globalState.error;
        pump.stop();
        digitalWrite(pinEnable, HIGH);
        heatOnOff(false);
        valveOnOff(false);
        candleOnOff(false);
        globalState.heater = false;
        lcd.writeStr("page error");
        if(dataLanguage.lang == dataLanguage.ua) {
            lcd.writeStr("error.t0.txt", "Перегрiв");   
            lcd.writeStr("error.t1.txt", "бака");  
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.ru) {
            lcd.writeStr("error.t0.txt", "Перегрев");   
            lcd.writeStr("error.t1.txt", "бака"); 
            lcd.writeStr("error.t2.txt", "");}
        if(dataLanguage.lang == dataLanguage.en) {
            lcd.writeStr("error.t0.txt", "High temp");   
            lcd.writeStr("error.t1.txt", "tank");
            lcd.writeStr("error.t2.txt", "");}
    }

}




void prcRefueling(void){ // заправка
    static int state = 0;
    static unsigned long timeOut = millis();
    if(globalState.mode == globalState.refueling){
        if(state == 0){
            timeOut = millis();
            valveOnOff(true);
            //digitalWrite(pinEnable, LOW);
            // pump.start(200, 500, mlMiToFreq(globalSetting[settingName.speedRefuel].val));
            //pump.setFrequency(mlMiToFreq(globalSetting[settingName.speedRefuel].val));
            state = 1;
            
        }
        if(state == 1){ 
            if(millis() - timeOut >= (globalSetting[settingName.timeRefuel].val) * 1000UL){ // тайм аут заправки
                //pump.stop();
                valveOnOff(false);
                //digitalWrite(pinEnable, HIGH);
                globalState.mode = globalState.error;
                lcd.writeStr("page error");
                if(dataLanguage.lang == dataLanguage.ua) {
                    lcd.writeStr("error.t0.txt", "Помилка");   
                    lcd.writeStr("error.t1.txt", "заправки");  
                    lcd.writeStr("error.t2.txt", "");}
                if(dataLanguage.lang == dataLanguage.ru) {
                    lcd.writeStr("error.t0.txt", "Ошибка");   
                    lcd.writeStr("error.t1.txt", "заправки"); 
                    lcd.writeStr("error.t2.txt", "");}
                if(dataLanguage.lang == dataLanguage.en) {
                    lcd.writeStr("error.t0.txt", "Error");   
                    lcd.writeStr("error.t1.txt", "refueling");
                    lcd.writeStr("error.t2.txt", "");}
            }


            if(digitalRead(pinFullTank) == LOW){ // полный бак
                //pump.stop();
                valveOnOff(false);
                digitalWrite(pinEnable, HIGH);
                globalState.mode = globalState.idle;
                pump.counter =  globalSetting[settingName.tankCapasity].val;
                EEPROM.put(1200, pump.counter);
                EEPROM.commit();
                lcd.writeStr("page error");
                if(dataLanguage.lang == dataLanguage.ua) {
                    lcd.writeStr("error.t0.txt", "Повний");   
                    lcd.writeStr("error.t1.txt", "бак");  
                    lcd.writeStr("error.t2.txt", "");}
                if(dataLanguage.lang == dataLanguage.ru) {
                    lcd.writeStr("error.t0.txt", "Полный");   
                    lcd.writeStr("error.t1.txt", "бак"); 
                    lcd.writeStr("error.t2.txt", "");}
                if(dataLanguage.lang == dataLanguage.en) {
                    lcd.writeStr("error.t0.txt", "Full");   
                    lcd.writeStr("error.t1.txt", "tank");
                    lcd.writeStr("error.t2.txt", "");}
            }

        }
    }
    else{
        state = 0;
    }
}