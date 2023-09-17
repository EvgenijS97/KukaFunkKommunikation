#ifndef _UI_ICON_H
#define _UI_ICON_H

#include <Arduino.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

struct Icon{
    Elegoo_TFTLCD * _tft;
    int16_t _width, _height;
    uint16_t _primC, _secC;
    Icon(){};
    Icon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC,uint16_t secC){
        _tft = tft;
        _width = width;
        _height = height;
        _primC = primC;
        _secC = secC;
    }
    virtual void draw(uint16_t x, uint16_t y) = 0;
};


struct HomeIcon : public Icon{
    HomeIcon(){};
    HomeIcon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC = 0xFFFF, uint16_t secC = 0x0000):
        Icon(tft, width, height, primC, secC)
    {}
    void draw(uint16_t x, uint16_t y){
        _tft->fillRect(x + (_width*2)/10, y + _height/2, (_width*6)/10, _height/2 - _height/6, _primC);
        _tft->fillRect(x + (_width*4)/10, y + _height/2 + (_height*1)/6, (_width*2)/10, (_height*1)/6, _secC);
        _tft->fillTriangle(x + (_width*1)/10, y + _height/2, x + (_width*9)/10, y + _height/2, x + _width/2, y + _height/6, _primC);
    }
};

struct SettingsIcon : public Icon{
    SettingsIcon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC,uint16_t secC):
        Icon(tft, width, height, primC, secC)
    {}
    void draw(uint16_t x, uint16_t y){
        
        
    }
};

struct FunkIcon : public Icon{
    FunkIcon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC,uint16_t secC):
        Icon(tft, width, height, primC, secC)
    {}
    void draw(uint16_t x, uint16_t y){
        
    }
};

struct RobotIcon : public Icon{
    RobotIcon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC,uint16_t secC):
        Icon(tft, width, height, primC, secC)
    {}
    void draw(uint16_t x, uint16_t y){
        
    }
};

struct WarningIcon : public Icon{
    WarningIcon(Elegoo_TFTLCD *tft, uint16_t width, uint16_t height, uint16_t primC,uint16_t secC):
        Icon(tft, width, height, primC, secC)
    {}
    void draw(uint16_t x, uint16_t y){
        
    }
};

#endif