#include "UI_Element.h"

class UI_Toggle : public UI_Element{
    protected:
    uint8_t _thicknes = 2; // Dicke des Rahmens
    uint16_t _frameColor; // Standdard weiß
    uint16_t _fillColor; // Orange
    uint16_t _circleColor;

    bool _status = false;
    uint32_t _time, _holdTime = 50000;

    public:
    UI_Toggle(){}
    UI_Toggle(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t background, 
        uint16_t frameColor = 0xFFFF, uint16_t fillColor = 0xFD20, uint16_t circleColor = 0x0000):
        UI_Element(tft, x, y, width, height, background),
        _frameColor(frameColor),
        _fillColor(fillColor),
        _circleColor(circleColor)
    {}
    ~UI_Toggle(){}

    void clear(){
        if(_act) _tft->fillRoundRect(_x, _y, _width, _height, _height/2, _background); 
    }
    void setStatus(bool status){
        _status = status;
    }
    void setHoldTime(uint32_t time){
        _holdTime = time;
    }
    
    void create(TSPoint p){
        if(_act){
            checkTouch(p);
            if(justPressed()) _time = micros();
            if(justReleased() && micros() - _time >= 500000){
                _update = true;
                _status = !_status;
            }
            if(_update || justPressed() || justReleased()){
                uint16_t color = _frameColor;
                if(isPressed()) color = 0xFD20;
                if(_lock) color = 0x7BEF;
                for(uint8_t i = 0; i < _thicknes; i++){
                    _tft->drawRoundRect(_x + i, _y + i, _width - i*2, _height - i*2, (_height - 2*i)/2, color);
                }
            }
            if(_update){
                int16_t relX = _x + _thicknes;
                int16_t relY = _y + _thicknes;
                int16_t relWidth = _width - _thicknes * 2;
                int16_t relHeight = _height - _thicknes * 2;
                uint8_t posX;
                uint8_t radius = (relHeight/2 - 2);
                uint16_t fill;

                if(!_status){
                    fill = _background;
                    posX = relX + 1 + radius;
                }else{
                    fill = _lock ? 0x000C : _fillColor;
                    posX = relX + relWidth - 2 - radius;
                }
                _tft->fillRoundRect(relX, relY, relWidth, relHeight, relHeight/2, fill);
                _tft->fillCircle(posX, _y + _height/2, radius, _lock ? 0x7BEF : _frameColor);
                
                _update = false;
            }
        }
    }
    bool getStatus(){
        return _status;
    }
};

class UI_CheckBox : public UI_Toggle{

};