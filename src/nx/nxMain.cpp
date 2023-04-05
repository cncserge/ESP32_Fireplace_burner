#include "global.h"
#include "EEPROM.h"
void trigger32(){    // printh 23 02 54 20 load main page

    lcd.writeStr("main.t1.txt", "");
    for(int i = 0; i < 7; i++){
        if(dataTimer.day[i]){
            lcd.writeStr("main.t1.txt", "!");
            break;
        }
    }




    if(globalState.mode == globalState.kindling){ // розжиг
        lcd.writeNum("main.j0.bpic", 28); /// прогресс
        lcd.writeNum("main.j0.ppic", 29); /// прогресс
        lcd.writeNum("main.p0.pic", 24); /// кнопка
    }

    if(globalState.mode == globalState.run){      // горит
        lcd.writeNum("main.p0.pic", 24); /// кнопка
        lcd.writeNum("main.p2.pic", 25); /// up
        lcd.writeNum("main.p1.pic", 26); /// dn
        lcd.writeNum("main.j0.val", 50); /// прогресс
    }
    if(globalState.mode == globalState.stop){ //стоп
        lcd.writeNum("main.p0.pic", 24); /// кнопка
        lcd.writeNum("main.p2.pic", 27); /// up
        lcd.writeNum("main.p1.pic", 27); /// dn
    }
    if(globalState.mode == globalState.idle){
        lcd.writeNum("main.p0.pic", 3); /// кнопка
        lcd.writeNum("main.p2.pic", 27); /// up
        lcd.writeNum("main.p1.pic", 27); /// dn
    }

    if(!globalState.onSound){
        lcd.writeNum("main.b2.pic", 8);
        lcd.writeNum("main.b2.pic2", 8);
    }
    else{
        lcd.writeNum("main.b2.pic", 7);
        lcd.writeNum("main.b2.pic2", 7);
    }
    lcd.writeStr("main.t0.txt", String(constrain( (int)(    map(pump.counter, 0, globalSetting[settingName.tankCapasity].val, 0, 100)    ) + 1, 0, 100) ) + "%" );


}
void trigger33(){    // printh 23 02 54 21 старт стоп
    beep = true;
    if(globalState.mode == globalState.idle){ //стоп
        globalState.mode = globalState.kindling;
        // lcd.writeNum("main.p0.pic", 24); /// кнопка
    }
    if(globalState.mode == globalState.run){
        globalState.mode = globalState.stop;
        // lcd.writeNum("main.p0.pic", 3); /// кнопка
        // lcd.writeNum("main.j0.bpic", 30); /// прогресс
        // lcd.writeNum("main.j0.ppic", 30); /// прогресс
    }
}
void trigger34(){    // printh 23 02 54 22 setting
beep = true;
    lcd.writeStr("page userSetting");
}
void trigger35(){    // printh 23 02 54 23 set date
beep = true;
    lcd.writeStr("page setDate");
}
void trigger36(){    // printh 23 02 54 24 set time
beep = true;
    lcd.writeStr("page setTime");
}
void trigger37(){    // printh 23 02 54 25 set language
beep = true;
    lcd.writeStr("page language");
}
void trigger38(){    // printh 23 02 54 26 ru
beep = true;
    dataLanguage.lang = dataLanguage.ru;
    lcd.writeStr("page main");
}
void trigger39(){    // printh 23 02 54 27 eng
beep = true;
    dataLanguage.lang = dataLanguage.en;
    lcd.writeStr("page main");
}
void trigger40(){    // printh 23 02 54 28 ua
beep = true;
    dataLanguage.lang = dataLanguage.ua;
    lcd.writeStr("page main");
}
void trigger41(){    // printh 23 02 54 29 sound on off
beep = true;
    if(globalState.onSound){
        lcd.writeNum("main.b2.pic", 8);
        lcd.writeNum("main.b2.pic2", 8);
        globalState.onSound = false;
        EEPROM.write(1100, 0);
        EEPROM.commit();
    }
    else{
        lcd.writeNum("main.b2.pic", 7);
        lcd.writeNum("main.b2.pic2", 7);
        globalState.onSound = true;
        EEPROM.write(1100, 1);
        EEPROM.commit();
    }
}
void trigger42(){    // printh 23 02 54 2A service
beep = true;
    if(globalState.mode == globalState.idle){
        lcd.writeStr("page pass");
    }
    
}
void trigger43(){    // printh 23 02 54 2B cancel pin code
beep = true;
    lcd.writeStr("page main");
}
void trigger44(){    // printh 23 02 54 2C next pin code // ввели правильный пин код
beep = true;
    if(globalState.mode == globalState.idle){
        lcd.writeStr("page service");
    }
}
void trigger45(){    // printh 23 02 54 2D load pass

}
void trigger46(){    // printh 23 02 54 2E load refueling

}
void trigger47(){    // printh 23 02 54 2F back refueling
beep = true;
    if(globalState.mode != globalState.refueling){
        lcd.writeStr("page main");
    }
}
void trigger48(){    // printh 23 02 54 30 main open refueling
beep = true;
    if(globalState.mode == globalState.idle){
        lcd.writeStr("page refueling");
    }
}
void trigger49(){    // printh 23 02 54 31 clear error
beep = true;
    globalState.mode = globalState.idle;
    lcd.writeStr("page main");
}
void trigger50(){    // printh 23 02 54 32 start stop refueling
beep = true;
    if(globalState.mode == globalState.idle){
        if(digitalRead(pinFullTank) == HIGH){
            globalState.mode = globalState.refueling;
            lcd.writeNum("refueling.p2.pic", 40);
        }
    }
    else{
        lcd.writeNum("refueling.p2.pic", 39);
        globalState.mode = globalState.idle;
        valveOnOff(false);
        pump.stop();
        digitalWrite(pinEnable, HIGH);
    }
}
void trigger51(){    // printh 23 02 54 33 up
beep = true;
    if(globalState.mode == globalState.run){
        
        if(globalState.power <= 90)
            globalState.power += 10;
    }
}
void trigger52(){    // printh 23 02 54 34 dn
beep = true;
    if(globalState.mode == globalState.run){
        blockHeatDn = true;
        if(globalState.power >= 10)
            globalState.power -= 10;
    }
}
void trigger53(){    // printh 23 02 54 35

}
void trigger54(){    // printh 23 02 54 36

}
void trigger55(){    // printh 23 02 54 37

}
void trigger56(){    // printh 23 02 54 38

}
void trigger57(){    // printh 23 02 54 39

}
void trigger58(){    // printh 23 02 54 3A

}
void trigger59(){    // printh 23 02 54 3B

}
void trigger60(){    // printh 23 02 54 3C

}
void trigger61(){    // printh 23 02 54 3D

}
void trigger62(){    // printh 23 02 54 3E

}
void trigger63(){    // printh 23 02 54 3F

}