#ifndef _MYUNOLIBRARY_H
#define _MYUNOLIBRARY_H

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <SD.h>
#include <Elegoo_TFTLCD.h> 

/**
 * Zum Zählen von Ziffern in einem Integer
 * @param value: Zahl (integer)
 * @return Gibt Anzahl der Ziffern zurück
*/
uint8_t digits(uint32_t value) {
	uint8_t n = 0;
	do{
		value /= 10;
		n++;
	} while (value > 0);
	return n;
}
/**
 * Ziffer in Buchstabe umwandel
 * @param z: Einstellige Ziffer
 * @return Buchstabe
*/
char to_char(uint8_t z){
    switch(z){
    case 0: return '0';
        break;
    case 1: return '1';
        break;
    case 2: return '2';
        break;
    case 3: return '3';
        break;
    case 4: return '4';
        break;
    case 5: return '5';
        break;
    case 6: return '6';
        break;
    case 7: return '7';
        break;
    case 8: return '8';
        break;
    case 9: return '9';
        break;
    default:
        return 'n';
    break;
    }
}
/**
 * Buchstabe in Ziffer umwandel
 * @param c: Buchstabe
 * @return Ziffer
*/
uint8_t to_int(char c){
    switch(c){
    case '0': return 0;
        break;
    case '1': return 1;
        break;
    case '2': return 2;
        break;
    case '3': return 3;
        break;
    case '4': return 4;
        break;
    case '5': return 5;
        break;
    case '6': return 6;
        break;
    case '7': return 7;
        break;
    case '8': return 8;
        break;
    case '9': return 9;
        break;
    default:
        return 255;
    break;
    }
}
/**
 * Wandelt eine Zahl (integer) in ein const char* um
 * @param value: Zahl (integer)
 * @return String
*/
String to_string(uint32_t value) {
	uint8_t n = digits(value);
	String temp;
	for (uint8_t i = n; i > 0; i--) {
		uint8_t a = (uint8_t)(value / pow(10, i - 1));
        temp += to_char(a);
		value -= a * pow(10, i - 1);
	}
	return temp;
}

bool timer(bool &bTime, uint32_t &nTimeStamp, uint16_t ms){
    // Zeitmessung Start
    if(!bTime){
        nTimeStamp = millis();
        bTime = true;
    }

    // Zeitmessung zurücksetzten
    if(millis() - nTimeStamp >= ms){
        bTime = false;
        return true;
    }else{
        return false;
    }    
}

/**
 * Timer in Millisekunden
 * @param time: Hier wird der Zeitstempel gespeichert. Muss global definiert werden.
 * @param ms: Zeit in Millisekunden
 * @return Gibt periodisch ein "true" nach der eingegebenen Zeit zurück
*/
bool timerMilli(uint32_t &time, uint16_t ms){
    if(abs(millis() - time) >= ms){
        time = millis();
        return true;
    }
    return false;
}

/**
 * Timer in Mikrosekunden
 * @param time: Hier wird der Zeitstempel gespeichert. Muss global definiert werden.
 * @param us: Zeit in Mikrosekunden
 * @return Gibt periodisch ein "true" nach der eingegebenen Zeit zurück
*/
bool timerMicro(uint32_t &time, uint16_t us){
    if(abs(micros() - time) >= us){
        time = micros();
        return true;
    }
    return false;
}

bool click(uint8_t TAST){
    if(digitalRead(TAST) == LOW){
        while(digitalRead(TAST) == LOW);
        return true;
    }
    return false;
}

void flipflop(bool *bTast, uint8_t TAST){
    if(digitalRead(TAST) == LOW){
        if(*bTast == false){
            *bTast = true;
        }
        else{
            *bTast = false;
        }
        //*bTast = !bTast;
    }
    while(digitalRead(TAST) == LOW){}
}

void flip(bool *bBool){
    if(*bBool == false){
        *bBool = true;
    }
    else{
        *bBool = false;
    }
}

void incDec(int &nVar, int nMax, int nInterv, uint8_t TastInc, uint8_t TastDec){
    while(digitalRead(TastInc) == LOW){
        if(nVar + nInterv <= nMax){
            nVar += nInterv;
        }
        
        while(digitalRead(TastInc) == LOW){}
        //delay(20);
    }

    while(digitalRead(TastDec) == LOW){
        if(nVar - nInterv >= 0){
            nVar -= nInterv;
        }
        while(digitalRead(TastDec) == LOW){}
        //delay(20);
    }
}

struct Taster{
    private:
    uint8_t _tast;
    bool laststate = false; 
    bool currstate = false; 
    public:
    Taster(uint8_t tast){
        _tast = tast;
        pinMode(_tast, INPUT_PULLUP);
    };
    void checkTouch(){
        if(digitalRead(_tast) == LOW){
            laststate = currstate;
            currstate = true;
        }else{
            laststate = currstate;
            currstate = false;
        }
    }
    bool isPressed() { return currstate; }
    bool justPressed() { return (currstate && !laststate); }
    bool justReleased() { return (!currstate && laststate); }
};

#define UNTOUCHED (uint8_t)0
#define CLICKED (uint8_t)1
#define PRESSED (uint8_t)2
uint8_t pressed(uint8_t TAST){
    if(digitalRead(TAST) == LOW){
        uint32_t start = 0;
        uint32_t end = 0;
        start = millis();
        while(digitalRead(TAST) == LOW);
        end = millis();
        if(end - start > 250){
            return PRESSED;
        }else{
            return CLICKED;
        }
    }
    return UNTOUCHED;
}

int random(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

struct v2d{
    uint16_t x, y;
};
struct Time{
    uint8_t s, m, h;
    uint16_t d, ms;
    uint32_t a;
};

Time millisToTime(uint64_t milli){
    Time t;
    t.a = floor(milli / 31536000000);
    milli %= 31536000000;

    t.d = floor(milli / 86400000);
    milli %= 86400000;

    t.h = floor(milli / 3600000);
    milli %= 3600000;

    t.m = floor(milli / 6000);
    milli %= 6000;

    t.s = floor(milli / 1000);
    milli %= 1000;

    t.ms = milli;    
    return t;
}

v2d vector60(uint8_t t, uint16_t l){
    float a = map(t, 0, 60, 0, 2 * PI * 1000)/1000;
    uint16_t x = uint16_t(sin(a) * l);
    uint16_t y = uint16_t(cos(a) * l);
    return {x, y};
}
v2d vector24(uint8_t t, uint16_t l){
    float a = map(t, 0, 24, 0, 2 * PI * 1000)/1000;
    uint16_t x = uint16_t(sin(a) * l);
    uint16_t y = uint16_t(cos(a) * l);
    return {x, y};
}
#endif