#include "global.h"
#include "EEPROM.h"

// void setDay(uint8_t day){
//     beep = true;
//     // if(dataTimer.day){
//     //     dataTimer.day = 0;
//     // }
//     // else{
//     //     lcd.writeStr("defferedstart.b"+String(day + 2)+".bco=40147");
//     //     lcd.writeStr("defferedstart.b"+String(day + 2)+".bco2=40147");
//     //     dataTimer.day |=  (0b10000000 >> day);
//     // }

//     if(dataTimer.day & (0b10000000 >> day)){
//         dataTimer.day &=  ~(0b10000000 >> day);
//         lcd.writeStr("defferedstart.b"+String(day + 2)+".bco=12710");
//         lcd.writeStr("defferedstart.b"+String(day + 2)+".bco2=12710");
//     }
//     else{
//         lcd.writeStr("defferedstart.b"+String(day + 2)+".bco=40147");
//         lcd.writeStr("defferedstart.b"+String(day + 2)+".bco2=40147");
//         dataTimer.day |=  (0b10000000 >> day);
//     }
// }

void setDay(uint8_t day){
    for(int i = 0; i < 7; i++){
        if(i == day){
            if(dataTimer.day[i]){
                dataTimer.day[i] = false;
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=12710");
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=12710");
            }
            else{
                dataTimer.day[i] = true;
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=40147");
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=40147");                
            }
        }
        else{
            dataTimer.day[i] = false;
            lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=12710");
            lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=12710");
        }
    }
}




void trigger0(){    // printh 23 02 54 00  butt timer global page
beep = true;
    lcd.writeStr("page defferedstart");
}
void trigger1(){    // printh 23 02 54 01 load page defferedstart
beep = true;
        for(int i = 0; i < 7; i++){
            if(dataTimer.day[i]){
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=40147");
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=40147");
            }
            else{
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=12710");
                lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=12710");
            }
        }
    // if(dataTimer.day != 0){
    //     uint8_t mask = 0b10000000;
    //     for(int i = 0; i < 7; i++){
    //         if(dataTimer.day & (mask >> i)){
    //             lcd.writeStr("defferedstart.b"+String(i + 2)+".bco=40147");
    //             lcd.writeStr("defferedstart.b"+String(i + 2)+".bco2=40147");
    //         }
    //     }
    // }
}
void trigger2(){    // printh 23 02 54 02 cancel all page
beep = true;
    lcd.writeStr("page main");
}
void trigger3(){    // printh 23 02 54 03 пн
beep = true;
    setDay(0);
    // if(dataTimer.day & 0b10000000){
    //     dataTimer.day &=  0b01111111;
    //     lcd.writeStr("defferedstart.b2.bco=12710");
    //     lcd.writeStr("defferedstart.b2.bco2=12710");
    // }
    // else{
    //     lcd.writeStr("defferedstart.b2.bco=40147");
    //     lcd.writeStr("defferedstart.b2.bco2=40147");

    //     dataTimer.day |=  0b10000000;
    // }
}
void trigger4(){    // printh 23 02 54 04 вт
beep = true;
    setDay(1);
}
void trigger5(){    // printh 23 02 54 05 ср
beep = true;
    setDay(2);
}
void trigger6(){    // printh 23 02 54 06 чт
beep = true;
    setDay(3);
}
void trigger7(){    // printh 23 02 54 07 пт
beep = true;
    setDay(4);
}
void trigger8(){    // printh 23 02 54 08 сб 
beep = true;
    setDay(5);
}
void trigger9(){    // printh 23 02 54 09 нд
beep = true;
    setDay(6);
}
void trigger10(){    // printh 23 02 54 0A open page defferedtime
    beep = true;
    for(int i = 0; i < 7; i++){
        if(dataTimer.day[i]){
            lcd.writeStr("page defferedtime");
            return;
        }
    }
    EEPROM.put(500, dataTimer);
    EEPROM.commit();
    lcd.writeStr("page main");
}
void trigger11(){    // printh 23 02 54 0B load page defferedtime
beep = true;
    lcd.writeStr("defferedtime.t6.txt", String(dataTimer.onHour));
    lcd.writeStr("defferedtime.t5.txt", String(dataTimer.onMinure));
}
void trigger12(){    // printh 23 02 54 0C hour up
beep = true;
    if(dataTimer.onHour < 23){
        dataTimer.onHour++;
        lcd.writeStr("defferedtime.t6.txt", String(dataTimer.onHour));
    }
}
void trigger13(){    // printh 23 02 54 0D hour dn
beep = true;
    if(dataTimer.onHour > 0){
        dataTimer.onHour--;
        lcd.writeStr("defferedtime.t6.txt", String(dataTimer.onHour));
    }
}
void trigger14(){    // printh 23 02 54 0E minute up
beep = true;
    if(dataTimer.onMinure < 59){
        dataTimer.onMinure++;
        lcd.writeStr("defferedtime.t5.txt", String(dataTimer.onMinure));
    }
}
void trigger15(){    // printh 23 02 54 0F minute dn
beep = true;
    if(dataTimer.onMinure > 0){
        dataTimer.onMinure--;
        lcd.writeStr("defferedtime.t5.txt", String(dataTimer.onMinure));
    }
}
void trigger16(){    // printh 23 02 54 10 open page differedsleep
beep = true;
    lcd.writeStr("page differedsleep");
}
void trigger17(){    // printh 23 02 54 11 load page differedsleep
beep = true;
    lcd.writeStr("differedsleep.t6.txt", dataLanguage.nameTime[dataLanguage.lang][dataTimer.offTimer]);
}
void trigger18(){    // printh 23 02 54 12 up timer off
beep = true;
    if(dataTimer.offTimer < 6 ){
        dataTimer.offTimer++;
        lcd.writeStr("differedsleep.t6.txt", dataLanguage.nameTime[dataLanguage.lang][dataTimer.offTimer]);
    }
}
void trigger19(){    // printh 23 02 54 13 dn timer off
beep = true;
        if(dataTimer.offTimer > 0 ){
        dataTimer.offTimer--;
        lcd.writeStr("differedsleep.t6.txt", dataLanguage.nameTime[dataLanguage.lang][dataTimer.offTimer]);
    }
}
void trigger20(){    // printh 23 02 54 14 save timer
    // сохранить таймер
    EEPROM.put(500, dataTimer);
    EEPROM.commit();
    beep = true;
    lcd.writeStr("page main");
}
void trigger21(){    // printh 23 02 54 15

}
void trigger22(){    // printh 23 02 54 16

}
void trigger23(){    // printh 23 02 54 17

}
void trigger24(){    // printh 23 02 54 18

}
void trigger25(){    // printh 23 02 54 19

}
void trigger26(){    // printh 23 02 54 1A

}
void trigger27(){    // printh 23 02 54 1B

}
void trigger28(){    // printh 23 02 54 1C

}
void trigger29(){    // printh 23 02 54 1D

}
void trigger30(){    // printh 23 02 54 1E

}
void trigger31(){    // printh 23 02 54 1F

}