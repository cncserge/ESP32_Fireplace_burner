#include "global.h"

DateTime locDt;
//////////////////////////////////// time

void trigger64(){    // printh 23 02 54 40 load page
    locDt = currDt;
    if(dataLanguage.lang == dataLanguage.ua) {lcd.writeStr("setTime.t0.txt", "Встановiть");   lcd.writeStr("setTime.t1.txt", "час");  lcd.writeStr("setTime.t2.txt", "Зберегти");lcd.writeStr("setTime.t3.txt", "Вiдмiна");}
    if(dataLanguage.lang == dataLanguage.ru) {lcd.writeStr("setTime.t0.txt", "Установите");   lcd.writeStr("setTime.t1.txt", "время"); lcd.writeStr("setTime.t2.txt", "Сохранить");lcd.writeStr("setTime.t3.txt", "Отмена");}
    if(dataLanguage.lang == dataLanguage.en) {lcd.writeStr("setTime.t0.txt", "Set");   lcd.writeStr("setTime.t1.txt", "time");lcd.writeStr("setTime.t2.txt", "Save");lcd.writeStr("setTime.t3.txt", "Cancel");}
    lcd.writeStr("setTime.t6.txt", String(locDt.hour()));
    lcd.writeStr("setTime.t5.txt", String(locDt.minute()));
    
}
void trigger65(){    // printh 23 02 54 41 save
    rtc.adjust(locDt);
    // currDt = locDt;
    lcd.writeStr("page main");
}
void trigger66(){    // printh 23 02 54 42 cancel
    lcd.writeStr("page main");
}
void trigger67(){    // printh 23 02 54 43 up min
    if(locDt.minute() < 59){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day(), locDt.hour(), locDt.minute() + 1, 0);
        lcd.writeStr("setTime.t5.txt", String(locDt.minute()));
    }
}
void trigger68(){    // printh 23 02 54 44 dn min
    if(locDt.minute() > 0){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day(), locDt.hour(), locDt.minute() - 1, 0);
        lcd.writeStr("setTime.t5.txt", String(locDt.minute()));
    }
}
void trigger69(){    // printh 23 02 54 45 up hour
    if(locDt.hour() < 23){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day(), locDt.hour() + 1, locDt.minute(), 0);
        lcd.writeStr("setTime.t6.txt", String(locDt.hour()));
    }
}
void trigger70(){    // printh 23 02 54 46 dn hour
    if(locDt.hour() > 0){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day(), locDt.hour() - 1, locDt.minute(), 0);
        lcd.writeStr("setTime.t6.txt", String(locDt.hour()));
    }
}


//////////////////////////////////////////////////////date

void trigger71(){    // printh 23 02 54 47 load
    locDt = currDt;
    if(dataLanguage.lang == dataLanguage.ua) {lcd.writeStr("setDate.t0.txt", "Встановiть дату");   lcd.writeStr("setDate.t2.txt", "Зберегти"); lcd.writeStr("setDate.t3.txt", "Вiдмiна");}
    if(dataLanguage.lang == dataLanguage.ru) {lcd.writeStr("setDate.t0.txt", "Установите дату");   lcd.writeStr("setDate.t2.txt", "Сохранить");lcd.writeStr("setDate.t3.txt", "Отмена");}
    if(dataLanguage.lang == dataLanguage.en) {lcd.writeStr("setDate.t0.txt", "Set date");          lcd.writeStr("setDate.t2.txt", "Save");     lcd.writeStr("setDate.t3.txt", "Cancel");}
    lcd.writeStr("setDate.t6.txt", String(locDt.year()));
    lcd.writeStr("setDate.t5.txt", String(locDt.month()));
    lcd.writeStr("setDate.t1.txt", String(locDt.day()));
}
void trigger72(){    // printh 23 02 54 48 save
    //currDt = DateTime(locDt.year(), locDt.month(), locDt.day(), currDt.hour(), currDt.minute(), currDt.second());
    rtc.adjust(DateTime(locDt.year(), locDt.month(), locDt.day(), currDt.hour(), currDt.minute(), currDt.second()));
    lcd.writeStr("page main");
}
void trigger73(){    // printh 23 02 54 49 cancel
    lcd.writeStr("page main");
}
void trigger74(){    // printh 23 02 54 4A year up
    if(locDt.year() < 2050){
        locDt = DateTime(locDt.year() + 1, locDt.month(), locDt.day(), currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t6.txt", String(locDt.year()));
    }
}
void trigger75(){    // printh 23 02 54 4B yaer dn
    if(locDt.year() > 2020){
        locDt = DateTime(locDt.year() - 1, locDt.month(), locDt.day(), currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t6.txt", String(locDt.year()));
    }
}
void trigger76(){    // printh 23 02 54 4C month up
    if(locDt.month() < 12){
        locDt = DateTime(locDt.year(), locDt.month() + 1, locDt.day(), currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t5.txt", String(locDt.month()));
    }
}
void trigger77(){    // printh 23 02 54 4D month dn
    if(locDt.month() > 1){
        locDt = DateTime(locDt.year(), locDt.month() - 1, locDt.day(), currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t5.txt", String(locDt.month()));
    }
}
void trigger78(){    // printh 23 02 54 4E day up
    if(locDt.day() < 31){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day() + 1, currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t1.txt", String(locDt.day()));
    }
}
void trigger79(){    // printh 23 02 54 4F day dn
    if(locDt.day() > 1){
        locDt = DateTime(locDt.year(), locDt.month(), locDt.day() - 1, currDt.hour(), currDt.minute(), currDt.second());
        lcd.writeStr("setDate.t1.txt", String(locDt.day()));
    }
}
