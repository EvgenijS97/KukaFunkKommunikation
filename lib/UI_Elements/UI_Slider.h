#include "UI_Element.h"
#include "MyUnoLibrary.h"

// Slider Klasse (funktioniert nur in Verbindung mit den Bibliotheken ELEGOO_GFX, ELEGOO_TFTLCD und TouchScreen)
class UI_Slider : public UI_Element{
    protected:
    uint8_t _thicknes = 2; // Dicke des Rahmens
    uint16_t _frameColor;
    uint16_t _fillColor;

    uint8_t _barWidth; // Nur ungerade Werte
    uint16_t _barColor = 0xFFFF;

    int16_t _oldPos;
    int16_t _value;

    int16_t relX;
    int16_t relY;
    int16_t relWidth;
    int16_t relHeight;
    int8_t barSide;

    int16_t relPosL;
    int16_t relPosR;
    int16_t relPosU;
    int16_t relPosD;

    int16_t relLeftWidth;
    int16_t relRightWidth;
    int16_t relUpHeight;
    int16_t relDownHeight;

    public:
    UI_Slider(){};
    UI_Slider(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t background, 
        uint16_t fillColor = 0x0000, uint16_t frameColor = 0xFFFF, uint8_t barWidth = 9):
        UI_Element(tft, x, y, width, height, background),
        _frameColor(frameColor),
        _fillColor(fillColor),        
        _barWidth(barWidth)
    {};

    void setFillColor(uint16_t color){
        _fillColor = color;
    }
    void setPos(int16_t pos){
        _oldPos = pos;
    }
    void setSliderW(uint8_t width){
        width%2 == 0 ? _barWidth = width-- : _barWidth = width;
    }
    virtual void setPos(uint8_t pos) = 0;

    virtual void create(TSPoint p) = 0;
    virtual uint8_t getValue() = 0;
};

class UI_SliderH : public UI_Slider{
    public:
    UI_SliderH(){};
    UI_SliderH(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t background, 
        uint16_t fillColor = 0xFFFF, uint16_t frameColor = 0xFFFF, uint8_t barWidth = 9):
        UI_Slider(tft, x, y, width, height, background, fillColor, frameColor, barWidth)
    {
        _value = _oldPos = _x + _width/2;
        if(_barWidth%2 == 0) _barWidth--;        
    };
    void setPos(uint8_t pos){
        relX = _x + _thicknes;
        relWidth = _width - _thicknes * 2;
        barSide = (_barWidth - 1) / 2;
        _value = _oldPos = map(pos, 0, 255, relX + barSide, relX + relWidth - barSide);
    }
    void create(TSPoint p){
        if(_act){
            checkTouch(p);
            if(_update || isPressed()){
                relX = _x + _thicknes;
                relY = _y + _thicknes;
                relWidth = _width - _thicknes * 2;
                relHeight = _height - _thicknes * 2;
                barSide = (_barWidth - 1) / 2;

                relPosL = p.x - barSide;
                relPosR = p.x + barSide;
                relPosU = p.y - barSide;
                relPosD = p.y + barSide;

                relLeftWidth = relPosL - relX;
                relRightWidth = relX + relWidth - relPosR;
                relUpHeight = relPosU - relY;
                relDownHeight = relHeight + relY - relPosD;
            }
            if(_update){
                // Rahmen
                for(uint8_t i = 0; i < _thicknes; i++){
                    _tft->drawRect(_x + i, _y + i, _width - i*2, _height - i*2, _frameColor);
                }
                
                _tft->fillRect(relX, relY, relWidth, relHeight, _background);
                _tft->fillRect(relX, relY, _oldPos - barSide - relX, relHeight, _fillColor);
                _tft->fillRect(_oldPos - barSide, relY, _barWidth, relHeight, _frameColor); // Slider
                _update = false;
            }
            if(isPressed() && _oldPos != p.x){
                if(p.x >= relX + barSide && p.x <= relX + relWidth - barSide){
                    _value = p.x;
                }
                if(relPosL >= relX && relPosR <= relX + relWidth && _oldPos != p.x){
                    _tft->fillRect(relX, relY, relLeftWidth, relHeight, _fillColor);
                    _tft->fillRect(relPosR, relY, relRightWidth, relHeight, _background);
                    _tft->fillRect(relPosL, relY, _barWidth, relHeight, _frameColor); // Slider
                    _oldPos = p.x;
                }
            }
        }
    }
    uint8_t getValue(){
        if(_act){
            return map(_value, relX + barSide, relX + relWidth - barSide, 0, 255);
        }
        return 0;
    }
};
class UI_SliderV : public UI_Slider{
    public:
    UI_SliderV(){};
    UI_SliderV(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t background, 
        uint16_t fillColor = 0xFFFF, uint16_t frameColor = 0xFFFF, uint8_t barWidth = 9):
        UI_Slider(tft, x, y, width, height, background, fillColor, frameColor, barWidth)
    {
        _value = _oldPos = _y + _height/2;
        if(_barWidth%2 == 0) _barWidth--;        
    };
    void setPos(uint8_t pos){
        relY = _y + _thicknes;
        relHeight = _height - _thicknes * 2;
        barSide = (_barWidth - 1) / 2;
        _value = _oldPos = map(pos, 0, 255, relY + relHeight - barSide, relY + barSide);
    }
    void create(TSPoint p){
        if(_act){
            checkTouch(p);
            if(_update || isPressed()){
                relX = _x + _thicknes;
                relY = _y + _thicknes;
                relWidth = _width - _thicknes * 2;
                relHeight = _height - _thicknes * 2;
                barSide = (_barWidth - 1) / 2;

                relPosL = p.x - barSide;
                relPosR = p.x + barSide;
                relPosU = p.y - barSide;
                relPosD = p.y + barSide;

                relLeftWidth = relPosL - relX;
                relRightWidth = relX + relWidth - relPosR;
                relUpHeight = relPosU - relY;
                relDownHeight = relHeight + relY - relPosD;
            }   
            if(_update){
                // Rahmen
                for(uint8_t i = 0; i < _thicknes; i++){
                    _tft->drawRect(_x + i, _y + i, _width - i*2, _height - i*2, _frameColor);
                }
                _tft->fillRect(relX, relY, relWidth, relHeight, _background);
                _tft->fillRect(relX, _oldPos + barSide, relWidth, relHeight + relY - (_oldPos + barSide), _fillColor);
                _tft->fillRect(relX, _oldPos - barSide, relWidth, _barWidth, _frameColor); // Slider
                _update = false;
            }
            if(isPressed() && _oldPos != p.y){
                if(p.y >= relY + barSide && p.y <= relY + relHeight - barSide){
                    _value = p.y;
                }
                if(relPosU >= relY && relPosD <= relY + relHeight && _oldPos != p.y){
                    _tft->fillRect(relX, relY, relWidth, relUpHeight, _background); 
                    _tft->fillRect(relX, relPosD, relWidth, relDownHeight, _fillColor);
                    _tft->fillRect(relX, relPosU, relWidth, _barWidth, _frameColor); // Slider
                    _oldPos = p.y;
                }
            }
        }
    }
    uint8_t getValue(){
        if(_act){
            return map(_value, relY + relHeight - barSide, relY + barSide, 0, 255);
        }
        return 0;
    }
};

