#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "EasyNextionLibrary.h"
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "StepperMotor.h"


extern RTC_DS1307 rtc;
extern StepperMotor pump;
// #include "ESP32TimerInterrupt.h"
// extern ESP32Timer ITimer0;
// main.cpp
void prckindling(void);
void prcRun(void);
void prcStop(void);
void prcRefueling(void);
void prcError(void);
void heatOnOff(bool);
void candleOnOff(bool);
double Thermistor(int RawADC);
extern const int pinHeater;
extern const int pinCandle;
extern const int pinValve;
extern bool beep;
extern const int pinStep;
extern const int pinDir;
extern const int pinEnable;
extern const int pinFullTank;
extern const int pinLeak;
extern const int pinBeep;
extern const int pinLevelFault;
extern EasyNex lcd;
extern const bool debug;
extern DateTime currDt;
struct structTimer{
    bool day[7];
    uint8_t onHour;
    uint8_t onMinure;
    uint8_t offTimer;
};
extern structTimer dataTimer;

struct structLanguage{
    const char* nameTime[3][7] = {{"30 хвилин", "1 година", "2 години", "3 години", "4 години", "5 годин", "6 годин"}, 
                                  {"30 мин", "1 час", "2 часа", "3 часа", "4 часа", "5 часов", "6 часов"}, 
                                  {"30 min", "1 hour", "2 hour", "3 hour", "4 hour", "5 hour", "6 hour"}};
    enum{ua, ru, en};
    int lang;
};
extern structLanguage dataLanguage;

struct structGlobalState{
    enum{kindling, run, stop, idle, error, refueling};
    int mode;
    int power = 50;
    bool heater;
    bool onSound;
    int  currentTemperatureBurner; // температура горелки
    int  currentTemperatureTank;   // температура бачка
    int flameTemperature;          // температура датчика пламени
};
extern structGlobalState globalState;

struct structSettingName{
    enum {
            tankCapasity, // емкость бака
            timeRefuel, // время заправки
           speedRefuel, // скорость заправки
               stepsMl, // шагов на милилитр
          minPumpSpeed, // минимальная скорость
          maxPumpSpeed, // максимальная скорость
            tempBurner, // тепмература горелки поддерживаемая
         maxTempBurner, // максимальная температура горелки
         tempFlameSens, // температура наличия пламени
        // countFlameSens, // количество измерений фильтра наличия пламени
           maxTempTank, // максимальная температура бачка
        // adcRefuelLevel, // значение порога ацп датчикаполного бака
        //  adcLeakSensor, // значение порога aцп датчика разрыва трубки
             timeOnCandle, // Время работы свечи
            tempOffFan, // температура отключеня вентилятора
      kindlingTimePump, // розжиг время перед включением насоса
     kindlingPowerPump, // розжиг скорость насоса мл/мин
    // kindlingVolumePump, // розжиг количество жидкости перекачаной насосом
    kindlingTimeCandle, // розжиг время перед вколючением свечи
       kindlingTimeEnd, // розжиг время для перехода в режим работа
         stopPowerPump, // стоп скорость насоса мл/мин
          stopTimePump, // стоп количество жидкости перекачаной насосом
          heatMin,      // минимальный значение тена
          heatMax,      // максимальное значение тена
          blockHeatTime,// время блокировки нагрева









        calibrationPulse, // количество импульсов калибровки
        calibrationSpeed, // скорость калибровки
                    count};
    int currParam;
};
extern structSettingName settingName;

struct structSetting{
    long min;
    long max;
    long val;
    const char* name[2];
};

extern structSetting globalSetting[];

void valveOnOff(bool onOff);

unsigned long mlMiToFreq(long mlMi);
extern bool enMotor; // true motor on false motor off
extern bool blockHeatDn; // блокировка нагрева при нажатии на минус




class SoftPwm{
    public:
    SoftPwm(bool * _out, const uint16_t * const _period /*0.1 sec*/, const double * const _value /*procent*/) :
        out(_out), period(_period), value(_value){
    };
    void run(unsigned long millis){
        if(*period == 0 || *value < 0.1) {*out = false; return;}
        on_ms  = ((*period * 100) / 100) * (*value);
        off_ms = (*period * 100) - on_ms;
        if(*out){
            if(millis - timer >= on_ms){
                timer = millis;
                *out = false;
            }
            // Serial.println("on : " + String(on_ms));
            // Serial.println("off: " + String(off_ms));
        }
        else{
            if(millis - timer >= off_ms){
                timer = millis;
                *out = true;
            }
        }
    }
    private:
        bool *out;
        uint32_t on_ms;
        uint32_t off_ms;
        const uint16_t * const period;       // periol ms
        const double * const   value;       // millis true
        unsigned long timer;
};


#endif