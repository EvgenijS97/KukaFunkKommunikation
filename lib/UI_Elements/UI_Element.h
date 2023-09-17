#ifndef _UI_ELEMENT_H
#define _UI_ELEMENT_H

#include <Arduino.h>
#include <string.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library


// Form der Ecken
#define ROUND_CORNER false
#define SQUARE_CORNER true

class UI_Element{
    protected:
    Elegoo_TFTLCD *_tft; // Pointer der ELEGOO_TFTLCD Klasse
    int16_t _x; 
    int16_t _y; 
    int16_t _width;
    int16_t _height;
    uint16_t _background;

    bool _act = true;
    bool laststate = false; 
    bool currstate = false; 
    bool _update = true; 

    bool _lock = false;

    public:
    UI_Element(){};
    UI_Element(Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t background){
        _background = background;
        _tft = tft;
        _x = x;
        _y = y;
        _width = width;
        _height = height;
    }
    ~UI_Element(){};
    virtual void checkTouch(TSPoint p){
        if(p.z >= 10 && p.z <= 1000 && p.x >= _x && p.x <= _x + _width && p.y >= _y && p.y <= _y + _height && !_lock){
            laststate = currstate;
            currstate = true;
        }else{
            laststate = currstate;
            currstate = false;
        }
    }
    virtual bool isPressed() { return currstate; }
    virtual bool justPressed() { return (currstate && !laststate); }
    virtual bool justReleased() { return (!currstate && laststate); }
    
    virtual void clear(){
        if(_act) _tft->fillRect(_x, _y, _width, _height, _background);
    }
    virtual void setPosition(int16_t x, int16_t y){
        _x = x;
        _y = y;
        _update = true;
    }
    virtual void setDimension(uint16_t width, uint16_t height){
        _width = width;
        _height = height;
        _update = true;
    }
    
    virtual void activate(){
        if(!_act){
            _act = true;
            _update = true;
        }        
    }    
    virtual void update(){
        _update = true;
    }
    virtual void lock(bool lck){
        /*
        _lock = lck;
        _update = true;
        */
        if(_lock && !lck){
            _lock = lck;
            _update = true;
        } 
        if(!_lock && lck){
            _lock = lck;
            _update = true;
        } 
        
    }

    virtual void deactivate(){
        if(_act){    
            _act = false;
        }
    }
    
    virtual int16_t getX()     {   return _x;         }
    virtual int16_t getY()     {   return _y;         }
    virtual int16_t getWidth() {   return _width;     }
    virtual int16_t getHeight(){   return _height;    }    
};

#endif