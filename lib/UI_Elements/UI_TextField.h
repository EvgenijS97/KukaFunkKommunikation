#ifndef _UI_TEXTFIELD_H
#define _UI_TEXTFIELD_H

#include "UI_Element.h"

#define TEXT_LEFT 0
#define TEXT_RIGHT 2
#define TEXT_CENTER 1
#define TEXT_UP 0
#define TEXT_DOWN 2


class UI_TextField : public UI_Element{
    const char *_text;
    const char *_oldText;
    bool _frame;
    uint8_t _textSize;
    uint16_t _textColor;
    uint16_t _fillColor;
    uint8_t _alignmentX;
    uint8_t _alignmentY;
    uint8_t _thicknes = 1;
    public:
    UI_TextField(){}
    UI_TextField(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height,  uint16_t background, const char *text, 
        bool frame = true, uint8_t textSize = 2, uint16_t textColor = 0x0000, uint16_t fillColor = 0xC618, 
        uint8_t alignmentX = TEXT_CENTER, uint8_t alignmentY = TEXT_CENTER):
        UI_Element(tft, x, y, width, height, background),
        _text(text),
        _frame(frame),
        _textSize(textSize),
        _textColor(textColor),
        _fillColor(fillColor),
        _alignmentX(alignmentX),
        _alignmentY(alignmentY)
    {}
    ~UI_TextField(){}

    void setText(const char *text){
        _text = text;
    }
    void setTextSize(uint8_t size){
        _textSize = size;
    }
    void create(){
        if((_update || _oldText != _text) && _act){
            for(uint8_t i = 0; i < 1; i++){
                _tft->drawRoundRect(_x + i, _y + i, _width - i*2, _height - i*2, 5, 0xFFFF);
            }
            _tft->fillRoundRect(_x + _thicknes, _y + _thicknes, _width - _thicknes * 2, _height - _thicknes * 2, 5, _fillColor);

            uint8_t relX;
            uint8_t relY;
            uint8_t textWidth = strlen(_text) * 6 * _textSize;
            uint8_t textHeight = 7 * _textSize;
            switch (_alignmentX)
            {
            case 0:
                relX = _x + _thicknes + 1;
                break;
            case 1:
                relX = _x + (_width - textWidth)/2;
                break;
            case 2:
                relX  = _x + _width - _thicknes - 1 - textWidth;
                break;
            default:
                break;
            }
            switch (_alignmentY)
            {
            case 0:
                relY = _y + _thicknes + 1;
                break;
            case 1:
                relY = _y + (_height - textHeight)/2;
                break;
            case 2:
                relY = _y + _height - _thicknes - 1 - textHeight;
                break;
            default:
                break;
            }
            _tft->setCursor(relX, relY);
            _tft->setTextColor(_textColor);
            _tft->setTextSize(_textSize);
            _tft->print(_text);

            _oldText = _text;
            _update = false;
        }
    }
};

#endif