#include "global.h"


bool startStop = false;
long speed, level;
const long maxSpeed = 10000L;
const long maxLevel = 10000L;

double saveCounter = 0;
double endCount = 0;



void trigger128(){    // printh 23 02 54 80 -1
    if(startStop) return;
    if(speed > 1) speed -= 1;
    lcd.writeStr("pump.t1.txt", String(speed));
}
void trigger129(){    // printh 23 02 54 81 - 10
    if(startStop) return;
    if(speed > 10) speed -= 10;
    lcd.writeStr("pump.t1.txt", String(speed));
}
void trigger130(){    // printh 23 02 54 82  - 100
    if(startStop) return;
    if(speed > 100) speed -= 100;
    lcd.writeStr("pump.t1.txt", String(speed));
}
void trigger131(){    // printh 23 02 54 83 1
    if(startStop) return;
    if(speed < maxSpeed - 1) speed += 1;
    lcd.writeStr("pump.t1.txt", String(speed));
}
void trigger132(){    // printh 23 02 54 84 10
    if(startStop) return;
    if(speed < maxSpeed - 10) speed += 10;
    lcd.writeStr("pump.t1.txt", String(speed));
}
void trigger133(){    // printh 23 02 54 85 100
    if(startStop) return;
    if(speed < maxSpeed - 100) speed += 100;
    lcd.writeStr("pump.t1.txt", String(speed));
}
//////////////////////////////////////////////////////////////////////////
void trigger134(){    // printh 23 02 54 86 -1
    if(startStop) return;
    if(level > 1) level -= 1;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger135(){    // printh 23 02 54 87 -10 
    if(startStop) return;
    if(level > 10) level -= 10;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger136(){    // printh 23 02 54 88 -100
    if(startStop) return;
    if(level > 100) level -= 100;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger137(){    // printh 23 02 54 89 1
    if(startStop) return;
    if(level < maxLevel - 1) level += 1;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger138(){    // printh 23 02 54 8A 10
    if(startStop) return;
    if(level < maxLevel - 10) level += 10;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger139(){    // printh 23 02 54 8B 100
    if(startStop) return;
    if(level < maxLevel - 100) level += 100;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
}
void trigger140(){    // printh 23 02 54 8C back
    if(!startStop){
        lcd.writeStr("page main");
    }
}
void trigger141(){    // printh 23 02 54 8D start
    if(!startStop){
        valveOnOff(false);
        pump.counterStep = 0.0;
        startStop = true;
        digitalWrite(pinEnable, LOW);
        pump.setFrequency(speed);
    }
}
void trigger142(){    // printh 23 02 54 8E stop
    if(startStop){
        //lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
        endCount = pump.counterStep;
        lcd.writeStr("pump.t5.txt", String((endCount / level) * 10) );
        startStop = false;
        digitalWrite(pinEnable, HIGH);
        pump.setFrequency(0);
    }
}
void trigger143(){    // printh 23 02 54 8F load
    startStop = false;
    lcd.writeStr("pump.t3.txt", String(level));
    lcd.writeStr("pump.t1.txt", String(speed));
}