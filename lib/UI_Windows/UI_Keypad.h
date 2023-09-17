#ifndef _UI_KEYPAD_H
#define _UI_KEYPAD_H

#include <Arduino.h>
#include <string.h>
#include "MyUnoLibrary.h"
#include "UI_Buttons.h"
#include "UI_TextField.h"
#include "UI_Window.h"

class UI_Keypad : public UI_Window{
    String _text;
    UI_TextField textField;
    UI_Button btC;
    UI_Button btCE;
    UI_Button ent;
    UI_Button esc;
    UI_Button number[10];


    const char* fillChars[10] =  {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

    public:
    UI_Keypad(){}
    UI_Keypad(Elegoo_TFTLCD *tft, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t background):
        UI_Window(tft, x, y, width, height, background)
    {
        _text.reserve(16);
        _text = "";
        uint8_t btWidth = _width/5;
        uint8_t btHeight = _height/7;

        uint8_t textSize;
        if(btHeight < 16 || btWidth < 14){
            textSize = 1;
        }else{
            textSize = 2;
        }
        textField.setTextSize(textSize);
        btC.setTextSize(textSize);
        btCE.setTextSize(textSize);
        for(uint8_t i = 0; i < 10; i++){
            number[i].setTextSize(textSize);
        }
        esc.setTextSize(textSize);
        ent.setTextSize(textSize);

        uint8_t gapeX = (_width - btWidth*3)/4;
        uint8_t gapeY = (_height - btHeight*6)/7;

        uint16_t relX = _x + gapeX;
        uint16_t relY = _y + gapeY;

        textField = UI_TextField(_tft, relX, relY, _width - 2*gapeX, btHeight, background, "", true, 2, 0xFFFF, 0x0000, TEXT_LEFT);

        btC = UI_Button(_tft, relX, relY + btHeight + gapeY, (width - 3*gapeX)/2, btHeight, background, "C");
        btCE = UI_Button(_tft, relX + gapeX + (width - 3*gapeX)/2, relY + btHeight + gapeY, (width - 3*gapeX)/2, btHeight, background, "CE");

        for(uint8_t i = 0; i < 3; i++){
            for(uint8_t j = 0; j < 3; j++){                
                number[i * 3 + j] = UI_Button(_tft, relX + j*(btWidth + gapeX), relY + (i + 2)*(btHeight + gapeY), btWidth, btHeight, background, fillChars[i * 3 + j]);
            }
        }

        esc = UI_Button(_tft, relX + 0*(btWidth + gapeX), relY + 5*(btHeight + gapeY), btWidth, btHeight, background, "\\/");
        number[3 * 3 + 0] = UI_Button(_tft, relX + 1*(btWidth + gapeX), relY + 5*(btHeight + gapeY), btWidth, btHeight, background, fillChars[3 * 3 + 0]);
        ent = UI_Button(_tft, relX + 2*(btWidth + gapeX), relY + 5*(btHeight + gapeY), btWidth, btHeight, background, "<-'");
    }
    void update(){
        textField.update();
        btC.update();
        btCE.update();
        for(uint8_t i = 0; i < 10; i++){
            number[i].update();
        }
        esc.update();
        ent.update();
    }

    void clear(){
        textField.clear();
        btC.clear();
        btCE.clear();
        for(uint8_t i = 0; i < 10; i++){
            number[i].clear();
        }
        esc.clear();
        ent.clear();        
    }

    void create(TSPoint p){
        textField.create();
        btC.create(p);
        btCE.create(p);
        for(uint8_t i = 0; i < 10; i++){
            number[i].create(p);
        }
        esc.create(p);
        ent.create(p); 

        for(uint8_t i = 0; i < 10; i++){
            if(number[i].justReleased()){
                textField.update();
                if(i != 9){
                    _text += to_char(i + 1);
                }else{
                    _text += to_char(0);
                }
            }
        }
        if(btC.justReleased()){
            _text.remove(_text.length() - 1);
            textField.update();
        } 
        if(btCE.justReleased()){
            _text = "";
            textField.update();
        } 
            
        textField.setText(_text.c_str());
    }

    int32_t getValue(){
        return (int32_t)_text.toDouble();
    }
    bool escape(){
        return esc.justReleased();
    }
    bool enter(){
        return ent.justReleased();
    }
    

};

#endif