#ifndef _UI_BUTTON_H
#define _UI_BUTTON_H

#include "UI_Element.h"
#include "UI_Icons.h"
#include <SD.h>

// Knopf Klasse (funktioniert nur in Verbindung mit den Bibliotheken ELEGOO_GFX, ELEGOO_TFTLCD und TouchScreen)
class UI_Button : public UI_Element{
    uint8_t _thicknes = 1; // Dicke des Rahmens
    uint16_t _frameColor = 0xFFFF; // Standdard weiß
    uint16_t _fillColor = 0x001F; // Standdard blau
    bool _cornerType = ROUND_CORNER; // Rund oder eckig
    const char* _label;
    uint16_t _textColor = 0xFFFF; // Standdard weiß
    uint8_t _textSize = 2;

    public:
    UI_Button(){};
    // Konstruktor
    UI_Button(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height,  uint16_t background, const char* label, 
        uint8_t textSize = 2, uint16_t textColor = 0xFFFF, uint8_t thicknes = 1, uint16_t frameColor = 0xFFFF, 
        uint16_t fillColor = 0xC618, bool cornerType = ROUND_CORNER):

        UI_Element(tft, x, y, width, height, background),
        _thicknes(thicknes),
        _frameColor(frameColor),
        _fillColor(fillColor),
        _cornerType(cornerType),
        _label(label),
        _textColor(textColor),
        _textSize(textSize)
    {};
    ~UI_Button(){}

    private:
    // Prüft ob der Knopf gedrückt wurde
    void touchEvent(TSPoint p){
        checkTouch(p);

        if(justPressed() || justReleased()){
            _update = true;
        }
    }

    public: 
    void setTextSize(uint8_t size){
        _textSize = size;
    }

    void clear(){
        if(_act){
            if(_cornerType == SQUARE_CORNER){
                _tft->fillRect(_x, _y, _width, _height, _background);
            }else{
                _tft->fillRoundRect(_x, _y, _width, _height, 5, _background);
            }
        }
    }
    // Zeichnet einen Knopf auf dem LCD
    void create(TSPoint p){
        if(_act){
            touchEvent(p);
            if(_update){
                
                uint16_t fill, text;
                if(!isPressed()){
                    fill = _fillColor;
                    text = _textColor;
                }else{
                    fill =  _textColor;
                    text = _fillColor;
                }

                if(_cornerType == SQUARE_CORNER){
                    _tft->fillRect(_x, _y, _width, _height, fill);
                    _tft->drawRect(_x, _y, _width, _height, _frameColor);
                }else{
                    _tft->fillRoundRect(_x, _y, _width, _height, 5, fill);
                    _tft->drawRoundRect(_x, _y, _width, _height, 5, _frameColor);
                }
                _tft->setCursor(_x + (_width - strlen(_label) * 6 * _textSize)/2, _y + (_height - 7 * _textSize)/2);
                _tft->setTextColor(text);
                _tft->setTextSize(_textSize);
                _tft->print(_label);
                _update = false;
            }
        }
    }      

};

/*
template<typename T>
class UI_IconButton : public UI_Element{
    T _icon;
    uint8_t _thicknes; // Dicke des Rahmens
    uint16_t _frameColor; 
    bool _cornerType; // Rund oder eckig

    public:
    UI_IconButton(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height,  uint16_t background, 
        uint8_t thicknes = 1, uint16_t frameColor = 0xFFFF, bool cornerType = ROUND_CORNER):

        UI_Element(tft, x, y, width, height, background),
        _thicknes(thicknes),
        _frameColor(frameColor),
        _cornerType(cornerType)
    {
        _icon = T(tft, width - thicknes, height - thicknes);
    };
    void create(TSPoint p){
        if(_act){
            checkTouch(p);

            if(_update || justPressed() || justReleased()){
                uint16_t color;
                if(isPressed()){
                    color = 0xFD20;
                }else{
                    color = _frameColor;
                }
                for(uint8_t i = 0; i < _thicknes; i++){
                    if(_cornerType == SQUARE_CORNER) _tft->drawRect(_x + i, _y + i, _width - i*2, _height - i*2, color);
                    if(_cornerType == ROUND_CORNER) _tft->drawRoundRect(_x + i, _y + i, _width - i*2, _height - i*2, 5, color);
                }
            }

            if(_update){
                _icon.draw(_x + _thicknes, _y + _thicknes);
            }
            _update = false;
        }
    }
};
*/

#define BUFFPIXEL 20
class UI_BMP_Button : public UI_Element{
    const char *_filename;
    uint8_t _thicknes; // Dicke des Rahmens
    uint16_t _frameColor; 
    bool _cornerType; // Rund oder eckig



    public:
    UI_BMP_Button(
        Elegoo_TFTLCD *tft, int16_t x, int16_t y, uint16_t width, uint16_t height,  uint16_t background, const char *filename, 
        uint8_t thicknes = 1, uint16_t frameColor = 0xFFFF, bool cornerType = ROUND_CORNER):

        UI_Element(tft, x, y, width, height, background),
        _filename(filename),
        _thicknes(thicknes),
        _frameColor(frameColor),
        _cornerType(cornerType)
    {};
    private:
    uint16_t read16(File f) {
        uint16_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read(); // MSB
        return result;
    }

    uint32_t read32(File f) {
        uint32_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read();
        ((uint8_t *)&result)[2] = f.read();
        ((uint8_t *)&result)[3] = f.read(); // MSB
        return result;
    }
    public:
    void create(TSPoint p){
        if(_act){
            checkTouch(p);

            if(_update || justPressed() || justReleased()){
                uint16_t color;
                if(isPressed()){
                    color = 0xFD20;
                }else{
                    color = _frameColor;
                }
                for(uint8_t i = 0; i < _thicknes; i++){
                    if(_cornerType == SQUARE_CORNER) _tft->drawRect(_x + i, _y + i, _width - i*2, _height - i*2, color);
                    if(_cornerType == ROUND_CORNER) _tft->drawRoundRect(_x + i, _y + i, _width - i*2, _height - i*2, 5, color);
                }
            }
            if(_update){
                File     bmpFile;
                int      bmpWidth, bmpHeight;   // W+H in pixels
                uint8_t  bmpDepth;              // Bit depth (currently must be 24)
                uint32_t bmpImageoffset;        // Start of image data in file
                uint32_t rowSize;               // Not always = bmpWidth; may have padding
                uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
                uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
                uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
                boolean  goodBmp = false;       // Set to true on valid header parse
                boolean  flip    = true;        // BMP is stored bottom-to-top
                //uint32_t startTime = millis();
                uint8_t  lcdidx = 0;
                boolean  first = true;

                if((_x >= _tft->width()) || (_y >= _tft->height())) return;

                Serial.println();
                Serial.print(F("Loading image '"));
                Serial.print(_filename);
                Serial.println('\'');
                // Open requested file on SD card
                if ((bmpFile = SD.open(_filename)) == NULL) {
                    Serial.println(F("File not found"));
                    _update = false;
                    return;
                }

                // Parse BMP header
                if(read16(bmpFile) == 0x4D42) { // BMP signature
                    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
                    (void)read32(bmpFile);  // Read & ignore creator bytes
                    bmpImageoffset = read32(bmpFile); // Start of image data
                    //Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
                    // Read DIB header
                    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
                    bmpWidth  = read32(bmpFile);
                    bmpHeight = read32(bmpFile);
                    if(read16(bmpFile) == 1) { // # planes -- must be '1'
                        bmpDepth = read16(bmpFile); // bits per pixel
                        Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
                        if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

                            goodBmp = true; // Supported BMP format -- proceed!
                            //Serial.print(F("Image size: "));
                            //Serial.print(bmpWidth);
                            //Serial.print('x');
                            //Serial.println(bmpHeight);

                            // BMP rows are padded (if needed) to 4-byte boundary
                            rowSize = (bmpWidth * 3 + 3) & ~3;

                            // If bmpHeight is negative, image is in top-down order.
                            // This is not canon but has been observed in the wild.
                            if(bmpHeight < 0) {
                            bmpHeight = -bmpHeight;
                            flip      = false;
                            }

                            // Crop area to be loaded
                            uint16_t relX = _x + (_width - bmpWidth)/2;
                            uint16_t relY = _y + (_height - bmpHeight)/2;

                            // Set TFT address window to clipped image bounds
                            _tft->setAddrWindow(relX, relY, relX + bmpWidth - 1, relY + bmpHeight - 1);

                            for (int row = 0; row < bmpHeight; row++) {
                                // Seek to start of scan line.  It might seem labor-
                                // intensive to be doing this on every line, but this
                                // method covers a lot of gritty details like cropping
                                // and scanline padding.  Also, the seek only takes
                                // place if the file position actually needs to change
                                // (avoids a lot of cluster math in SD library).
                                uint32_t pos;
                                if(flip){ // Bitmap is stored bottom-to-top order (normal BMP)
                                    pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                                }else{     // Bitmap is stored top-to-bottom
                                    pos = bmpImageoffset + row * rowSize;
                                }
                                if(bmpFile.position() != pos) { // Need seek?
                                    bmpFile.seek(pos);
                                    buffidx = sizeof(sdbuffer); // Force buffer reload
                                }

                                for (int col = 0; col < bmpWidth; col++) {
                                    // Time to read more pixel data?
                                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                                        // Push LCD buffer to the display first
                                        if(lcdidx > 0) {
                                            _tft->pushColors(lcdbuffer, lcdidx, first);
                                            lcdidx = 0;
                                            first  = false;
                                        }
                                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                                        buffidx = 0; // Set index to beginning
                                    }

                                    // Convert pixel from BMP to TFT format
                                    uint8_t b = sdbuffer[buffidx++];
                                    uint8_t g = sdbuffer[buffidx++];
                                    uint8_t r = sdbuffer[buffidx++];
                                    lcdbuffer[lcdidx++] = _tft->color565(r,g,b);
                                }
                            }
                            // Write any remaining data to LCD
                            if(lcdidx > 0) {
                            _tft->pushColors(lcdbuffer, lcdidx, first);
                            } 
                            //Serial.print(F("Loaded in "));
                            //Serial.print(millis() - startTime);
                            //Serial.println(" ms");
                        }
                    }
                }

                bmpFile.close();
                if(!goodBmp) Serial.println(F("BMP format not recognized."));
                
                _update = false;
            }                                                 
        }
    }
};
#undef BUFFPIXEL

#endif