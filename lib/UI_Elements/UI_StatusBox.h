#include "UI_Element.h"
#include <string.h>
// StatusBox Klasse (funktioniert nur in Verbindung mit den Bibliotheken ELEGOO_GFX, ELEGOO_TFTLCD und TouchScreen)
class UI_Status : public UI_Element{

};

class UI_StatusBox : public UI_Element{
    protected:
    uint8_t _thicknes = 1; // Dicke des Rahmens
    uint16_t _frameColor;
    uint16_t _onColor = 0x07E0; // Standard Grün
    uint16_t _offColor = 0xF800; // Standard Rot
    uint16_t _textColor;
    uint8_t _textSize;
    bool _cornerType;

    bool _status = false;
    bool _oldStatus = true;

    public:
    UI_StatusBox(){}
    UI_StatusBox(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, uint16_t height, uint16_t background,
        uint16_t frameColor = 0xFFFF, uint16_t textColor = 0xFFFF, uint8_t textSize = 1, bool cornerType = SQUARE_CORNER):

        UI_Element(tft, x, y, width, height, background),
        _frameColor(frameColor),
        _textColor(textColor),
        _textSize(textSize),
        _cornerType(cornerType)
    {}
    ~UI_StatusBox(){}

    void clear(){
        if(_act){
            if(_cornerType == SQUARE_CORNER){
                _tft->fillRect(_x, _y, _width, _height, _background);
            }else{
                _tft->fillRoundRect(_x, _y, _width, _height, 5, _background);
            }
        }        
    }
    void setOnColor(uint16_t color){
        _onColor = color;
    }
    void setOffColor(uint16_t color){
        _offColor = color;
    }
    void setStatus(bool status){
        _status = status;
    }

    void create(bool status){
        _status = status;
        if((_update || _status != _oldStatus) && _act){

            uint16_t _fillColor;
            const char *_label;
            if(status){
                _label = "EIN";
                _fillColor = _lock ? 0x0320 : _onColor;
            }else{
                _label = "AUS";
                _fillColor = _lock ? 0x6000 : _offColor;
            }
            
            if(_cornerType == SQUARE_CORNER){
                _tft->fillRect(_x, _y, _width, _height, _fillColor);
                _tft->drawRect(_x, _y, _width, _height, _lock ? 0x7BEF : _frameColor);
            }else{
                _tft->fillRoundRect(_x, _y, _width, _height, 5, _fillColor);
                _tft->drawRoundRect(_x, _y, _width, _height, 5, _lock ? 0x7BEF : _frameColor);
            }
            _tft->setCursor(_x + (_width - strlen(_label) * 6 * _textSize)/2, _y + (_height - 7 * _textSize)/2);
            _tft->setTextColor(_lock ? 0x7BEF : _textColor);
            _tft->setTextSize(_textSize);
            _tft->print(_label);
            _oldStatus = _status;
            _update = false;
        }
    }
};

class UI_StatusText : public UI_Element{
    protected:
    uint8_t _thicknes = 1; // Dicke des Rahmens
    uint16_t _frameColor;
    uint16_t _onColor = 0x07E0; // Standard Grün
    uint16_t _offColor = 0xF800; // Standard Rot
    uint16_t _textColor;
    uint8_t _textSize;
    bool _cornerType;

    bool _status = false;
    bool _oldStatus = true;
    String _text;

    public:
    UI_StatusText(){}
    UI_StatusText(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, uint16_t height, uint16_t background,
        uint16_t frameColor = 0xFFFF, uint16_t textColor = 0xFFFF, uint8_t textSize = 1, bool cornerType = SQUARE_CORNER):

        UI_Element(tft, x, y, width, height, background),
        _frameColor(frameColor),
        _textColor(textColor),
        _textSize(textSize),
        _cornerType(cornerType)
    {}
    ~UI_StatusText(){}

    void clear(){
        if(_act){
            if(_cornerType == SQUARE_CORNER){
                _tft->fillRect(_x, _y, _width, _height, _background);
            }else{
                _tft->fillRoundRect(_x, _y, _width, _height, 5, _background);
            }
        }        
    }
    void setOnColor(uint16_t color){
        _onColor = color;
    }
    void setOffColor(uint16_t color){
        _offColor = color;
    }
    void setStatus(bool status){
        _status = status;
    }
    void setText(String text){
        _text = text;
    }

    void create(bool status){
        _status = status;
        if((_update || _status != _oldStatus) && _act){

            uint16_t _fillColor, txtColor;
            if(status){
                txtColor = 0x0000;        
                _fillColor = _lock ? 0x0320 : _onColor;
            }else{
                txtColor = _textColor;
                _fillColor = _lock ? 0x6000 : _offColor;
            }
            
            if(_cornerType == SQUARE_CORNER){
                _tft->fillRect(_x, _y, _width, _height, _fillColor);
                _tft->drawRect(_x, _y, _width, _height, _lock ? 0x7BEF : _frameColor);
            }else{
                _tft->fillRoundRect(_x, _y, _width, _height, 5, _fillColor);
                _tft->drawRoundRect(_x, _y, _width, _height, 5, _lock ? 0x7BEF : _frameColor);
            }
            _tft->setCursor(_x + (_width - strlen(_text.c_str()) * 6 * _textSize)/2, _y + (_height - 7 * _textSize)/2);
            _tft->setTextColor(_lock ? 0x7BEF : txtColor);
            _tft->setTextSize(_textSize);
            _tft->print(_text);
            _oldStatus = _status;
            _update = false;
        }
    }
};

class UI_ArrowH : public UI_Element{
    private:
    uint8_t _thicknes;
    uint16_t _onColor;
    uint16_t _offColor;

    bool _status = false;
    bool _oldStatus = true;

    public:
    UI_ArrowH(){}
    UI_ArrowH(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, uint16_t height, uint16_t background,
        uint8_t thicknes, uint16_t onColor, uint16_t offColor):
        UI_Element(tft, x, y, width, height, background),
        _thicknes(thicknes),
        _onColor(onColor),
        _offColor(offColor)
    {}
    ~UI_ArrowH(){}
    void create(bool status){
        _status = status;
        if((_update || _status != _oldStatus) && _act){

            _tft->drawFastHLine(_x, _y, _width, status ? _onColor : _offColor);
            for(uint8_t i = 0; i < _thicknes; i++){
                _tft->drawFastHLine(_x, _y + i, _width - i, status ? _onColor : _offColor);
                _tft->drawFastHLine(_x, _y - i, _width - i, status ? _onColor : _offColor);
                
                for(uint8_t j = 0; j < 2; j++){
                    _tft->drawLine(_x + _width - 2*i + j, _y, _x + _width - (_height/2 - 1) - 2*i + j, _y - (_height/2 - 1), status ? _onColor : _offColor);

                    _tft->drawLine(_x + _width - 2*i + j, _y, _x + _width - (_height/2 - 1) - 2*i + j, _y + (_height/2 - 1), status ? _onColor : _offColor);
                } 
            } 
            _oldStatus = _status;
            _update = false; 
        }
    }
};