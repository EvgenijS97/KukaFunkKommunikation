#ifndef _UI_WINDOW_H
#define _UI_WINDOW_H

#include <Arduino.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include "UI_Elements.h"

struct Node{
    int16_t x, y;
};

class UI_Window{    
    protected:
    Elegoo_TFTLCD *_tft;
    int16_t _x;
    int16_t _y;
    int16_t _width;
    int16_t _height;
    uint16_t _background;
    Node *_node;

    bool _act = true;
    bool _buttonPressed = false;
    bool _update = true;

    UI_Window(){};
    UI_Window(Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t background){
        _background = background;
        _tft = tft;
        _x = x;
        _y = y;
        _width = width;
        _height = height;
    };
    ~UI_Window(){};
    
    void clear(){
        if(_act) _tft->fillRect(_x, _y, _width, _height, _background);
    }
    void setPosition(int16_t x, int16_t y){
        _x = x;
        _y = y;
        _update = true;
    }
    void setDimension(uint16_t width, uint16_t height){
        _width = width;
        _height = height;
        _update = true;
    }
    void activate(){
        if(!_act){
            _act = true;
            _update = true;
        }
    }
    void update(){
        _update = true;
    }


    void deactivate(){
        if(_act){    
            _act = false;
        }
    }
    int16_t getX()     {   return _x;         }
    int16_t getY()     {   return _y;         }
    int16_t getWidth() {   return _width;     }
    int16_t getHeight(){   return _height;    } 
};

#endif 