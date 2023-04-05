#include "EEPROM.h"
#include "global.h"
// pin 7437

void trigger80(){    // printh 23 02 54 50 load service
    settingName.currParam = 0;
}
void trigger81(){    // printh 23 02 54 51 load pump

}
void trigger82(){    // printh 23 02 54 52 load setting
    lcd.writeStr("setting.t0.txt", globalSetting[settingName.currParam].name[0]);
    lcd.writeStr("setting.t1.txt", globalSetting[settingName.currParam].name[1]);
    lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
}
void trigger83(){    // printh 23 02 54 53 pump speed ++

}
void trigger84(){    // printh 23 02 54 54 pump speed --

}
void trigger85(){    // printh 23 02 54 55 pump pulse ++

}
void trigger86(){    // printh 23 02 54 56 pump pulse --

}
void trigger87(){    // printh 23 02 54 57 pump start

}
void trigger88(){    // printh 23 02 54 58 pump stop

}
void trigger89(){    // printh 23 02 54 59 pump back
    lcd.writeStr("page main");
}
void trigger90(){    // printh 23 02 54 5A previous param
    if(settingName.currParam > 0){
        settingName.currParam--;
    }
    else{
        settingName.currParam = settingName.count - 3;
    }
    lcd.writeStr("setting.t0.txt", globalSetting[settingName.currParam].name[0]);
    lcd.writeStr("setting.t1.txt", globalSetting[settingName.currParam].name[1]);
    lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
}
void trigger91(){    // printh 23 02 54 5B next param
    if(settingName.currParam < settingName.count - 3){
        settingName.currParam++;
    }
    else{
        settingName.currParam = 0;
    }
    lcd.writeStr("setting.t0.txt", globalSetting[settingName.currParam].name[0]);
    lcd.writeStr("setting.t1.txt", globalSetting[settingName.currParam].name[1]);
    lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
}
void trigger92(){    // printh 23 02 54 5C up 1
    if(globalSetting[settingName.currParam].val < globalSetting[settingName.currParam].max){
        globalSetting[settingName.currParam].val+=1;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger93(){    // printh 23 02 54 5D up 10
    if( (globalSetting[settingName.currParam].val + 10) < globalSetting[settingName.currParam].max){
        globalSetting[settingName.currParam].val += 10;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger94(){    // printh 23 02 54 5E up 100
    if( (globalSetting[settingName.currParam].val + 100) < globalSetting[settingName.currParam].max){
        globalSetting[settingName.currParam].val += 100;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger95(){    // printh 23 02 54 5F dn 1
    if( (globalSetting[settingName.currParam].val) > globalSetting[settingName.currParam].min){
        globalSetting[settingName.currParam].val -= 1;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger96(){    // printh 23 02 54 60 dn 10
    if( (globalSetting[settingName.currParam].val - 10) > globalSetting[settingName.currParam].min){
        globalSetting[settingName.currParam].val -= 10;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger97(){    // printh 23 02 54 61 dn 100
    if( (globalSetting[settingName.currParam].val - 100) > globalSetting[settingName.currParam].min){
        globalSetting[settingName.currParam].val -= 100;
        lcd.writeStr("setting.t2.txt", String(globalSetting[settingName.currParam].val));
    }
}
void trigger98(){    // printh 23 02 54 62 save 
    EEPROM.put(settingName.currParam * 10, globalSetting[settingName.currParam].val);
    EEPROM.commit();
    // long t = 0;
    // EEPROM.get(settingName.currParam * 10, t);
    // Serial.println("Adr :"+ String(settingName.currParam) + "Val : " + String(t));
}
void trigger99(){    // printh 23 02 54 63 back
    lcd.writeStr("page main");
}
void trigger100(){    // printh 23 02 54 64 open service setting

}
void trigger101(){    // printh 23 02 54 65 open pump calibration
    if(globalState.mode == globalState.idle){
        lcd.writeStr("page pump");
    }
}