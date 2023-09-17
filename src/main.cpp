#include <Arduino.h>
#include <EEPROM.h>

#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>
#include <SD.h>
#include <string.h>
#include "UI_Elements.h"
#include "UI_Windows.h"
#include "MyUnoLibrary.h"

#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <RF24Network.h>

// LCD PINS
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// NRF24L01 Pins
#define CE (uint8_t)44
#define CSN (uint8_t)42

// Andere Pins
#define TAST 24

// Assign human-readable names to some common 16-bit color values:
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
//#define PINK        0xF81F      /* 255,   0, 255 */

//Textgröße
#define TEXT_SMALL 1
#define TEXT_NORMAL 2
#define TEXT_LARGE 3

//Touchscreen Pins
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP (Touchscreen kalibrieren)
#define TS_MINX 134
#define TS_MAXX 913

#define TS_MINY 84
#define TS_MAXY 903

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Debugen 
#define DEBUG_MODE
#define SHOW_FPS

// Nachtmodus (zum deaktivieren "#define DARKMODE" auskommentiern)
#define DARKMODE 
#ifdef DARKMODE 
  #define BACKGROUND BLACK
  #define FRAMECOLOR WHITE
#else
  #define BACKGROUND WHITE
  #define FRAMECOLOR BLACK
#endif

struct Settings{
  bool darkmode = true;
  bool showAllNodes = false;
  bool enableNRF = true;
  bool fastSending = false;
  bool enableConnections = false;
};
struct Connection{
  uint8_t from, to;
  uint8_t inputPin, outputPin;
};

void receiving();
void sending();
void ping();
TSPoint readTouchScreen();
void listMenu();
void praktikumMenu();
void axisMenu();
void IOSettings();
void devMenu();
void settingsMenu();
uint8_t packBools8(bool *b);
void unpackBools8(uint8_t package, bool *b);
void printTouch(TSPoint p);

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

RF24 radio(CE, CSN);
RF24Network network(radio);

// Netzwerk Adressen
#define MASTER 00
#define KUKA1 01
#define KUKA2 02
#define KUKA3 03
#define IGUS1 04
#define IGUS2 05
#define LIN_AXIS IGUS2 + 10
#define THIS_NODE MASTER
const uint16_t address[6] = {KUKA1, KUKA2, KUKA3, IGUS1, IGUS2, LIN_AXIS};
const uint8_t nodes = 5;

// 0: kuka1 | 1: kuka2 | 2: kuka3 | 3: igus1 | 4: igus2 | 5: Linear Achse
bool inputs[nodes][8];
bool outputs[nodes][8];
const char text[6][13] = {"Kuka 1", "Kuka 2", "Kuka 3", "Igus 1", "Igus 2", "Linear Achse"};

bool toggleMem[nodes][8];

#define UPPER_BAR 20
#define LOWER_BAR 280

// Hauptmenü
UI_Button btListN(&tft, 30, 40, 180, 40, BACKGROUND, "Alle Roboter", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
UI_Button btP(&tft, 30, 85, 180, 40, BACKGROUND, "Praktikum", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
UI_Button btAxis(&tft, 30, 130, 180, 40, BACKGROUND, "Linear Achse", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
UI_Button btConnect(&tft, 30, 175, 180, 40, BACKGROUND, "Verbinden", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
UI_Button btDev(&tft, 30, 220, 180, 40, BACKGROUND, "Entwickler", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);

// Obere Leiste
UI_BMP_Button btWireless(&tft, 180, 0, 29, 20, DARKGREY, "Wireless.bmp", 1, DARKGREY);
// Untere Leiste
UI_BMP_Button btHome(&tft, 90, 280, 60, 40, DARKGREY, "Home.bmp", 1, DARKGREY);
UI_BMP_Button btSet(&tft, 200, 280, 40, 40, DARKGREY, "Settings.bmp", 1, DARKGREY);

// Einstellungen
Settings set;
bool enableConnections = true;

// FPS Counter
uint32_t timeStamp;
#ifdef SHOW_FPS
uint32_t startTime;
uint16_t elapsedTime = 10;
#endif

// Counter zum Senden (pro Iteration wird nur an ein Node gesendet)
uint8_t sendTo = 0;

// Gescheiterte Versuche
#define ATTEMPTS 3
uint8_t attempts[6] = {4,4,4,4,4,4};
bool nodeAvailable[nodes];

// Verbindungen
Connection *con;
uint8_t nConnections = 0;

void setup(void) {
  #ifdef DEBUG_MODE
    Serial.begin(9600);
  #endif

  for(uint8_t i = 0; i < nodes; i++){
    for(uint8_t j = 0; j < 8; j++){
      inputs[i][j] = false;
      outputs[i][j] = false;
      toggleMem[i][j] = false;
    }
  }
  
  //============ LCD ===============//
  tft.reset();
  uint16_t identifier = tft.readID();
  identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(1, 1);

  //======= Netzwerk Klasse ========//
  SPI.begin();
  if(radio.begin()){
    #ifdef DEBUG_MODE
    Serial.println(F("NRF Initialisierung erfolgreich"));
    tft.println(F("NRF Initialisierung erfolgreich"));
    #endif
  }else{
    #ifdef DEBUG_MODE
    Serial.println(F("NRF Initialisierung fehlgeschlagen"));
    tft.println(F("NRF Initialisierung fehlgeschlagen"));
    #endif
  }

  network.begin(90, THIS_NODE);
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setRetries(5,15);

  //============== SD ==============//
  if(SD.begin(10, 11, 12, 13)){
    #ifdef DEBUG_MODE
    Serial.println(F("SD Initialisierung erfolgreich"));
    tft.println(F("SD Initialisierung erfolgreich"));
    #endif
  }else{
    #ifdef DEBUG_MODE
    Serial.println(F("SD nicht gefunden"));
    tft.println(F("SD nicht gefunden"));
    #endif
  }

  // Prüfen ob Datei vorhanden ist
  File myFile;
  if(SD.exists("lesen.txt")){
    #ifdef DEBUG_MODE
    Serial.println(F("Datei \"lesen.txt\" gefunden"));
    tft.println(F("Datei \"lesen.txt\" gefunden"));
    #endif
    myFile = SD.open("lesen.txt", FILE_READ);
  }else{
    #ifdef DEBUG_MODE
    Serial.println(F("Datei \"Verbindungen.txt\" nicht gefunden"));
    tft.println(F("Datei \"Verbindungen.txt\" nicht gefunden"));
    #endif
  }  
  
  
  // Einlesen der Verbindungen von der SD
  if(myFile){
    uint32_t start = millis();
    String streamBuffer;
    streamBuffer.reserve(6);
    streamBuffer = myFile.readStringUntil(';');
    nConnections = streamBuffer.toInt();
    Serial.println(nConnections);
    con = new Connection[nConnections];
    
    const char textF[6][12] = {"Kuka1", "Kuka2", "Kuka3", "Igus1", "Igus2", "LinearAchse"};
    for(uint8_t i = 0; i < nConnections; i++){
      myFile.readStringUntil('\n');      

      streamBuffer = myFile.readStringUntil(':');
      for(uint8_t j = 0; j < nodes; j++){
        if(streamBuffer == textF[j]){
          con[i].from = j;
          break;
        }
      }

      streamBuffer = myFile.readStringUntil('>');      
      con[i].outputPin = streamBuffer.toInt();

      streamBuffer = myFile.readStringUntil(':');    
      for(uint8_t j = 0; j < nodes; j++){
        if(streamBuffer == textF[j]){
          con[i].to = j;
          break;
        }
      }

      streamBuffer = myFile.readStringUntil(';');
      con[i].inputPin = streamBuffer.toInt();
      
      #ifdef DEBUG_MODE
      Serial.print("From: "); Serial.print(con[i].from);
      Serial.print(",");  Serial.print(con[i].outputPin);
      
      Serial.print("  to: "); Serial.print(con[i].to);
      Serial.print(",");  Serial.println(con[i].inputPin);  
      #endif    
    }
    #ifdef DEBUG_MODE
    Serial.print(nConnections); Serial.print(" Verbindungen in "); Serial.print(millis() - start); Serial.println(" ms eingelesen");
    tft.print(nConnections); tft.print(" Verbindungen in "); tft.print(millis() - start); tft.println(" ms eingelesen");
    #endif 

    myFile.close(); 
  }

  pinMode(TAST, INPUT_PULLUP);
  #ifdef DEBUG_MODE
  tft.println(F("\nZum Fortfahren weissen Taster druecken"));
  while(digitalRead(TAST) == HIGH){}
  #endif 
  
  tft.fillScreen(BLACK);

  // Untere und obere Leiste
  tft.fillRect(0, 0, 240, 20, DARKGREY);
  btWireless.create({0,0,0});
  tft.fillRect(0, 280, 240, 40, DARKGREY);

  #ifdef SHOW_FPS
  tft.setCursor(0, 0);
  tft.setTextSize(TEXT_SMALL);
  tft.print(F("IPS: "));
  #endif
}


void loop() {
  #ifdef SHOW_FPS
  startTime = micros();  
  #endif
    
  // Netzwerk
  network.update(); 
  delay(2);

  // Empfangen
  receiving();

  // Verbinden
  if(set.enableConnections){
    for(uint8_t i = 0; i < nConnections; i++){
      inputs[con[i].to][con[i].inputPin] = outputs[con[i].from][con[i].outputPin];
    }
  }
  
  // Senden
  sending();  

  // Einlesen der Touchscreen Daten
  TSPoint p = readTouchScreen();

  // FPS + Nodes
  if(timerMilli(timeStamp, 250)){
    #ifdef SHOW_FPS
    tft.setCursor(30, 0);
    tft.setTextSize(TEXT_SMALL);
    tft.setTextColor(WHITE, DARKGREY);
    tft.print((uint16_t)(1000000/elapsedTime));
    tft.println("    ");
    #endif

    // Anzahl aktiver Nodes
    uint8_t n = 0;
    for(uint8_t i = 0; i < nodes; i++){
      if(attempts[i] <= ATTEMPTS) n++;
    }
    tft.setTextColor(WHITE, DARKGREY);
    tft.setTextSize(TEXT_NORMAL);
    tft.setCursor(220, 3);
    tft.println(n);
  } 

  //===== Hauptmenü =====//
  btListN.create(p);
  btP.create(p);
  btAxis.create(p);
  //btConnect.create(p);
  //btDev.create(p);

  btHome.create(p);
  btSet.create(p);

  if(btListN.justReleased())    listMenu();
  if(btP.justReleased())        praktikumMenu();
  if(btAxis.justReleased())     axisMenu();
  //if(btConnect.justReleased())  IOSettings();
  //if(btDev.justReleased())      devMenu();
  if(btSet.justReleased())      settingsMenu();    

  #ifdef SHOW_FPS
  elapsedTime = micros() - startTime;
  #endif
}

void receiving(){
  while(network.available()){
    RF24NetworkHeader header;
    uint8_t buffer;
    network.read(header, &buffer, sizeof(buffer)); // Read the incoming data
    
    for(uint8_t i = 0; i < nodes; i++){
      if(header.from_node == address[i]){
        #ifdef DEBUG_MODE
          Serial.print(F("Nachricht von: "));
          Serial.println(text[i]);
        #endif
        attempts[i] = 0;
        nodeAvailable[i] = true;
        
        unpackBools8(buffer, outputs[i]);
        break;
      }
    }
  }
}

void sending(){
  /*
  for(uint8_t i = 0; i < nodes; i++){
    if(attempts[i] <= ATTEMPTS){
      RF24NetworkHeader header(address[i]);
      uint8_t package = packBools8(inputs[i]);

      bool ok = network.write(header, &package, sizeof(package));
      #ifdef DEBUG_MODE
      if(ok){
        Serial.print(F("Gesendet an: "));
        Serial.println(text[i]);
      }
      #endif
      if(!ok) attempts[i]++;
    }
  }
  */
  
  
  if(attempts[sendTo] <= ATTEMPTS){
    RF24NetworkHeader header(address[sendTo]);
    uint8_t package = packBools8(inputs[sendTo]);

    bool ok = network.write(header, &package, sizeof(package));
    #ifdef DEBUG_MODE
    if(ok){
      Serial.print(F("Gesendet an: "));
      Serial.println(text[sendTo]);
    }
    #endif
    if(!ok) attempts[sendTo]++;
  }else{
    for(uint8_t i = 0; i < 8; i++){
      outputs[sendTo][i] = false;
    }
  }
  sendTo++;
  if(sendTo >= nodes) sendTo = 0;
}

void ping(){
  for(uint8_t i = 0; i < nodes; i++){
    RF24NetworkHeader header(address[i]);
    header.type = NETWORK_PING;
    uint8_t package;
    
    bool ok = false;
    for(uint8_t j = 0; j < 5; j++){
      network.update();
      ok = network.write(header, &package, sizeof(package));
      if(ok) break;
    }

    nodeAvailable[i] = ok ? true : false;
  }
}

TSPoint readTouchScreen(){
  pinMode(XP, INPUT);
  pinMode(XM, INPUT);
  pinMode(YM, INPUT);
  pinMode(YP, INPUT);

  TSPoint p = {0, 0, 0};
  if(ts.pressure() >= MINPRESSURE && ts.pressure() <= MAXPRESSURE){
    p = ts.getPoint();
    p.x = map(p.x, TS_MAXX, TS_MINX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    /*
    if(p.z >= MINPRESSURE && p.z <= MAXPRESSURE){
      for(uint8_t i = 0; i < 3; i++){
        p.x += map(ts.getPoint().x, TS_MAXX, TS_MINX, 0, 240);
        p.y += map(ts.getPoint().y, TS_MINY, TS_MAXY, 0, 320);
      }
      p.x /= 4;
      p.y /= 4;
    }
    */
  }  

  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YM, OUTPUT);
  pinMode(YP, OUTPUT);
  return p;
}

void listMenu(){
  UI_Button btLeft(&tft, 10, 20, 30, 30, BACKGROUND, "<", TEXT_NORMAL, BLACK);
  UI_TextField textBox(&tft, 42, 20, 156, 30, BACKGROUND, "Kuka 1");
  UI_Button btRight(&tft, 200, 20, 30, 30, BACKGROUND, ">", TEXT_NORMAL, BLACK);

  UI_Toggle toggleIO[nodes][8];

  // Ein-, Ausgänge Kuka Roboter (0: Eingänge, 1: Ausgänge)
  UI_StatusText statIO[2][8];

  // Menu
  int8_t robot = 0, oldRobot = 8;

  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
  
  #define OFFSET_TOGGLE 60
  for(uint8_t i = 0; i < 8; i++){
    for(uint8_t j = 0; j < nodes; j++){
      toggleIO[j][i] = UI_Toggle(&tft, 33, OFFSET_TOGGLE + i*27, 40, 23, BACKGROUND, WHITE, BLUE);
      toggleIO[j][i].setStatus(toggleMem[j][i]);
      if(j < 2) statIO[j][i] = UI_StatusText(&tft, 90 + j*95, OFFSET_TOGGLE + i*27, 40, 23, BACKGROUND, WHITE, WHITE, TEXT_NORMAL, ROUND_CORNER);
    }    
  }
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(8, OFFSET_TOGGLE + 5);
  tft.println(F("E:"));
  tft.setCursor(160, OFFSET_TOGGLE + 5);
  tft.println(F("A:"));

  tft.drawRoundRect(2, UPPER_BAR + 30 + 2, 140, LOWER_BAR - (UPPER_BAR + 30 + 2 + 2), 6, WHITE);
  tft.drawRoundRect(150, UPPER_BAR + 30 + 2, 85, LOWER_BAR - (UPPER_BAR + 30 + 2 + 2), 6, WHITE);
  
  uint8_t a = 0;
  while(attempts[robot] > ATTEMPTS && !set.showAllNodes && a <= nodes){
    robot++;
    if(robot >= nodes) robot = 0;
    a++;
  }
  if(a > nodes){
    textBox.setText("None");
  }else{
    textBox.setText(text[robot]);
  }  
  
  //====== Loop =======//
  bool run = true;
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif
    // Netzwerk
    network.update(); 
    delay(2);

    // Empfangen
    receiving();

    // Verbinden (oder Verknüpfung: Toggle oder Ausgang --> Eingang)
    for(uint8_t i = 0; i < 8; i++){
      inputs[robot][i] = toggleIO[robot][i].getStatus();
      statIO[1][i].setStatus(outputs[robot][i]);
    }   
    if(set.enableConnections){
      for(uint8_t i = 0; i < nConnections; i++){
        if(outputs[con[i].from][con[i].outputPin]) inputs[con[i].to][con[i].inputPin] = true;
      }
    } 
    

    // Senden
    sending(); 

    // FPS + Nodes
    if(timerMilli(timeStamp, 250)){
      #ifdef SHOW_FPS
      tft.setCursor(30, 0);
      tft.setTextSize(TEXT_SMALL);
      tft.setTextColor(WHITE, DARKGREY);
      tft.print((uint16_t)(1000000/elapsedTime));
      tft.println("    ");
      #endif

      // Anzahl aktiver Nodes
      uint8_t n = 0;
      for(uint8_t i = 0; i < nodes; i++){
        if(attempts[i] <= ATTEMPTS) n++;
      }
      tft.setTextColor(WHITE, DARKGREY);
      tft.setTextSize(TEXT_NORMAL);
      tft.setCursor(220, 3);
      tft.println(n);
    }       

    // TouchScreen
    TSPoint p = readTouchScreen();

    // Knöpfe
    btLeft.create(p);
    btRight.create(p);
    
    // Prüfen auf verfügbare Nodes
    if(btLeft.justReleased()){
      a = 0;
      do{
        robot--;
        if(robot < 0) robot = nodes - 1;
        a++;
      }while(attempts[robot] > ATTEMPTS && !set.showAllNodes && a <= nodes);
      if(a > nodes){
        textBox.setText("None");
      }else{
        textBox.setText(text[robot]);
      }
    } 
    if(btRight.justReleased()){
      a = 0;
      do{
        robot++;
        if(robot >= nodes) robot = 0;
        a++;
      }while(attempts[robot] > ATTEMPTS && !set.showAllNodes && a <= nodes);
      if(a > nodes){
        textBox.setText("None");
      }else{
        textBox.setText(text[robot]);
      }
    }

    a = 0;
    for(uint8_t i = 0; i < nodes; i++){
      if(attempts[i] > ATTEMPTS && !set.showAllNodes){
        a++;
      }else{
        break;
      }
    }
    if(a > nodes){
      textBox.setText("None");
      tft.fillRect(33, OFFSET_TOGGLE, 97, LOWER_BAR - (OFFSET_TOGGLE + 4), BACKGROUND);
      tft.fillRect(185, OFFSET_TOGGLE, 40, LOWER_BAR - (OFFSET_TOGGLE + 4), BACKGROUND);
    }     
    textBox.create();

    // Aktualisiere Toggles und Status
    if(oldRobot != robot && a < nodes){      
      for(uint8_t i = 0; i < 8; i++){
        String strL, strR;
        if(robot < 3){
          strL = String(i + 1);
          strR = String(i + 9);
        } 
        if(robot == 3){
          strL = String(i + 21);
          strR = String(i + 24);
        } 
        if(robot == 4){
          strL = strR = String(i + 21);
        } 
        if(robot > 2 && i > 3){
          tft.fillRect(33, OFFSET_TOGGLE + i*27, 97, LOWER_BAR - (OFFSET_TOGGLE + 4 + i*27), BACKGROUND);
          tft.fillRect(185, OFFSET_TOGGLE + i*27, 40, LOWER_BAR - (OFFSET_TOGGLE + 4 + i*27), BACKGROUND);
          break;
        }

        toggleIO[robot][i].update();
        statIO[0][i].setText(strL);
        statIO[1][i].setText(strR);        
        statIO[0][i].update();
        statIO[1][i].update();
      }
      oldRobot = robot;
    }
    
    if(a < nodes){
      for(uint8_t i = 0; i < 8; i++){        
        if(robot > 2 && i > 3) break;
        toggleIO[robot][i].create(p);        
        statIO[0][i].create(inputs[robot][i]);
        statIO[1][i].create(outputs[robot][i]);

        toggleMem[robot][i] = toggleIO[robot][i].getStatus();
      }
    }

    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }    
    
    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  }
  #undef OFFSET_TOGGLE
}

void praktikumMenu(){
  #define OFFSET 5
  #define INTERVALL (LOWER_BAR - UPPER_BAR - 2*OFFSET)/8

  uint8_t pages = ceil((float)nConnections / (float)8);

  UI_Button btUp(&tft, 220, UPPER_BAR + 5, 20, 20, BACKGROUND, "^", TEXT_NORMAL, BLACK, 1, LIGHTGREY, LIGHTGREY, SQUARE_CORNER);  
  UI_SliderV scroll(&tft, 220, UPPER_BAR + 27, 20, LOWER_BAR - (UPPER_BAR + 5 + 2 + 2 + 5 + 2*20), BACKGROUND, BLACK, LIGHTGREY, (LOWER_BAR - (UPPER_BAR + 2*OFFSET + 40 + 4 + 4))/pages);
  UI_Button btDown(&tft, 220, LOWER_BAR - 5 - 20, 20, 20, BACKGROUND, "v", TEXT_NORMAL, BLACK, 1, LIGHTGREY, LIGHTGREY, SQUARE_CORNER);
  
  UI_TextField textIO[2][8];
  UI_ArrowH arrow[8];
  
  int8_t page = 0, oldPage = 1;

  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
  for(uint8_t i = 0; i < 2; i++){
    for(uint8_t j = 0; j < 8; j++){
      textIO[i][j] = UI_TextField(&tft, 5 + 135*i, UPPER_BAR + OFFSET + j*INTERVALL, 70, 23, BACKGROUND, " ");
      if(i < 1) arrow[j] = UI_ArrowH(&tft, 80, UPPER_BAR + OFFSET + 11 + j*INTERVALL, 55, 23, BACKGROUND, 2, GREEN, LIGHTGREY);
    }
  }
  scroll.setPos(map(page, 0, pages - 1, 255, 0));

  //====== Loop =======//
  bool run = true;
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif

    // Netzwerk
    network.update(); 
    delay(2);

    // Empfangen
    receiving();

    // Verbinden
    if(set.enableConnections){
      for(uint8_t i = 0; i < nConnections; i++){
        inputs[con[i].to][con[i].inputPin] = outputs[con[i].from][con[i].outputPin];
      }
    }
    
      
    // Senden
    sending();

    // FPS + Nodes
    if(timerMilli(timeStamp, 250)){
      #ifdef SHOW_FPS
      tft.setCursor(30, 0);
      tft.setTextSize(TEXT_SMALL);
      tft.setTextColor(WHITE, DARKGREY);
      tft.print((uint16_t)(1000000/elapsedTime));
      tft.println("    ");
      #endif

      // Anzahl aktiver Nodes
      uint8_t n = 0;
      for(uint8_t i = 0; i < nodes; i++){
        if(attempts[i] <= ATTEMPTS) n++;
      }
      tft.setTextColor(WHITE, DARKGREY);
      tft.setTextSize(TEXT_NORMAL);
      tft.setCursor(220, 3);
      tft.println(n);
    }
    
    // LCD
    TSPoint p = readTouchScreen();
    
    btUp.create(p);
    btDown.create(p);
    scroll.create(p);

    if(btUp.justReleased() && page - 1 >= 0) page--;
    if(btDown.justReleased() && page + 1 < pages) page++;

    if(btUp.justReleased() || btDown.justReleased()){
      scroll.setPos(map(page, 0, pages - 1, 255, 0));
      scroll.update();
    }else{
      page = map(scroll.getValue(), 255, 0, 0, pages - 1);
    }

    if(oldPage != page){
      const char nodeT[5][3] = {"K1", "K2", "K3", "I1", "I2"};
      for(uint8_t i = 0; i < 8; i++){
        uint8_t indize = page*8;
        if(indize + i >= nConnections){
          tft.fillRect(5, UPPER_BAR + OFFSET + i*INTERVALL, 215, LOWER_BAR - (UPPER_BAR + OFFSET + (i)*INTERVALL), BACKGROUND);
          break;
        }
        String leftS, rightS;
        uint8_t a, b;
        switch (con[indize + i].from){
          case 0: a = 9;
            break;
          case 1: a = 9;
            break;
          case 2: a = 9;
            break;
          case 3: a = 24;
            break;
          case 4: a = 21;
            break;        
          default: a = 0;
            break;
        }
        switch (con[indize + i].to){
          case 0: b = 1;
            break;
          case 1: b = 1;
            break;
          case 2: b = 1;
            break;
          case 3: b = 21;
            break;
          case 4: b = 21;
            break;        
          default: b = 0;
            break;
        }
        
        leftS += nodeT[con[indize + i].from];
        leftS += ":";
        leftS += String(con[indize + i].outputPin + a);

        rightS += nodeT[con[indize + i].to];
        rightS += ":";
        rightS += String(con[indize + i].inputPin + b);
        
        textIO[0][i].setText(leftS.c_str());
        textIO[1][i].setText(rightS.c_str());

        textIO[0][i].update();
        textIO[1][i].update();

        textIO[0][i].create();
        textIO[1][i].create();

        arrow[i].update();  
      }
      oldPage = page;
    }

    for(uint8_t i = 0; i < 8; i++){
      if(page*8 + i >= nConnections) break;
      if(set.enableConnections){
        arrow[i].create(outputs[con[page*8 + i].from][con[page*8 + i].outputPin]);
      }else{
        arrow[i].create(false);
      }
    }
    


    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }

    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  }
  #undef OFFSET
  #undef INTERVALL
}

void axisMenu(){
  enum command {STOP = 70, FORWARD, BACKWARD, MOVETO, LOCATION, DONE, REV, RESET, CALIBRATE, PROPERTIES};

  UI_Button btBack(&tft, 10, UPPER_BAR + 10, 105, 40, BACKGROUND, "Zuruck", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
  UI_Button btForward(&tft, 125, UPPER_BAR + 10, 105, 40, BACKGROUND, "Vorwarts", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);

  UI_Button btReset(&tft, 10, UPPER_BAR + 50 + 10, 50, 40, BACKGROUND, "RST", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btStop(&tft, 80, UPPER_BAR + 50 + 10, 80, 40, BACKGROUND, "Stop", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);  
  UI_Button btPos(&tft, 180, UPPER_BAR + 50 + 10, 50, 40, BACKGROUND, "Pos", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);

  UI_Button btCal(&tft, 10, 185, 70, 40, BACKGROUND, "Cal", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btRev(&tft, 10, 235, 70, 40, BACKGROUND, "Ref", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btMove(&tft, 90, 235, 140, 40, BACKGROUND, "Fahre zu", TEXT_NORMAL, WHITE, 1, WHITE, MAROON, ROUND_CORNER); 

  UI_Keypad pad(&tft, 0, UPPER_BAR + 60, 240, LOWER_BAR - (UPPER_BAR + 60), BACKGROUND);

  struct Payload{
    uint8_t command;
    int32_t position;
  };

  int32_t sledgePosition = 0, oldPos = 1;

  bool run = true;
  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);  

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, UPPER_BAR + 60 + 40 + 10);
  tft.print("Position [inc]: ");

  //====== Loop =======//
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif
    
    // Netzwerk
    network.update(); 
    delay(2);
    
    // Empfangen
    while(network.available()){
      Payload buffer;
      RF24NetworkHeader header;      
      network.read(header, &buffer, sizeof(buffer)); // Read the incoming data 
      if(header.from_node == LIN_AXIS){
        if(buffer.command == LOCATION){
          sledgePosition = buffer.position;
          Serial.print("Schlitten bei: "); Serial.println(buffer.position);
        } 
        if(buffer.command == DONE){
          Serial.println("Aufgabe erledigt!");
          sledgePosition = buffer.position;
        } 
        #ifdef DEBUG_MODE
          //Serial.print(F("Nachricht von: ")); Serial.println(text[5]);
        #endif
        attempts[5] = 0;  
      }      
    }
    
    // FPS + Nodes
    if(timerMilli(timeStamp, 250)){
      #ifdef SHOW_FPS
      tft.setCursor(30, 0);
      tft.setTextSize(TEXT_SMALL);
      tft.setTextColor(WHITE, DARKGREY);
      tft.print((uint16_t)(1000000/elapsedTime));
      tft.println("    ");
      #endif

      // Anzahl aktiver Nodes
      uint8_t n = 0;
      for(uint8_t i = 0; i < nodes; i++){
        if(attempts[i] <= ATTEMPTS) n++;
      }
      tft.setTextColor(WHITE, DARKGREY);
      tft.setTextSize(TEXT_NORMAL);
      tft.setCursor(220, 3);
      tft.println(n);
    }

    // LCD
    TSPoint p = readTouchScreen();

    btBack.create(p);
    btForward.create(p);
    btStop.create(p);
    btPos.create(p);
    btMove.create(p);
    btRev.create(p);
    btReset.create(p);   
    btCal.create(p); 

    if(sledgePosition != oldPos){
      tft.setTextColor(WHITE, BACKGROUND);
      tft.setCursor(10, UPPER_BAR + 110 + 18);
      tft.setTextSize(2);
      tft.print(sledgePosition);
      tft.println("               ");
      oldPos = sledgePosition;
    }
    
    // Senden
    RF24NetworkHeader header(LIN_AXIS);
    Payload package;
    package.command = 0;

    if(btForward.justReleased()) package.command = FORWARD;
    if(btBack.justReleased()) package.command = BACKWARD;
    if(btStop.justReleased()) package.command = STOP;
    if(btMove.justReleased()){
      tft.fillRect(0, UPPER_BAR + 60, 240, LOWER_BAR - (UPPER_BAR + 60), BACKGROUND);
      pad.update();

      while(true){
        p = readTouchScreen();
        pad.create(p);
        if(pad.enter()){
          package.command = MOVETO;
        }else{
          package.command = 0;
        }
        if(pad.escape() || pad.enter()) break;
      }
      pad.clear();
      p = {0,0,0};
      package.position = pad.getValue();

      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(10, UPPER_BAR + 60 + 40 + 10);
      tft.print("Position: ");

      btReset.update();
      btStop.update();
      btPos.update();
      btRev.update();
      btMove.update();
      btCal.update();

    } 
    if(btPos.justReleased()) package.command = LOCATION;
    if(btRev.justReleased()) package.command = REV;
    if(btReset.justReleased()) package.command = RESET;
    if(btCal.justReleased()) package.command = CALIBRATE;
    
    bool ok = false;
    uint32_t ms = millis();
    while(package.command != 0 && millis() - ms <= 2000){
      network.update();
      ok = network.write(header, &package, sizeof(package));
      if(ok) break;      
    }    

    #ifdef DEBUG_MODE
    if(ok){
      Serial.print(F("Gesendet an: ")); Serial.println(text[5]);      
    }
    #endif    
    
    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }

    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  }
}

void IOSettings(){
  enum command {STOP = 70, FORWARD, BACKWARD, MOVETO, LOCATION, DONE, REV, RST};

  UI_Button btRev(&tft, 20, 130, 200, 40, BACKGROUND, "Referenzieren", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btSkip(&tft, 20, UPPER_BAR + 20 + 40 + 10, 200, 40, BACKGROUND, "Uberspringen", TEXT_NORMAL, WHITE, 1, WHITE, MAROON, ROUND_CORNER);

  UI_Button btReset(&tft, 20, UPPER_BAR + 50 + 10, 50, 40, BACKGROUND, "RST", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);

  UI_Button btBack(&tft, 10, UPPER_BAR + 10, 105, 40, BACKGROUND, "Zuruck", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btForward(&tft, 125, UPPER_BAR + 10, 105, 40, BACKGROUND, "Vorwarts", TEXT_NORMAL, WHITE, 1, WHITE, NAVY, ROUND_CORNER);
  UI_Button btStop(&tft, 80, UPPER_BAR + 50 + 10, 80, 40, BACKGROUND, "Stop", TEXT_NORMAL, WHITE, 1, WHITE, MAROON, ROUND_CORNER);  
  UI_Button btPos(&tft, 180, UPPER_BAR + 50 + 10, 50, 40, BACKGROUND, "Pos", TEXT_NORMAL, WHITE, 1, WHITE, DARKGREEN, ROUND_CORNER);
  UI_Button btMove(&tft, 50, 235, 140, 40, BACKGROUND, "Fahre zu", TEXT_NORMAL, WHITE, 1, WHITE, MAROON, ROUND_CORNER); 

  UI_Button btInc;
  UI_Button btDec;

  struct Payload{
    uint8_t command;
    int32_t position;
  };

  int32_t sledgePosition = 0, oldPos = 1;

  bool run = true;
  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);  

  //====== Loop =======//
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif
    
    // Netzwerk
    network.update(); 
    delay(2);
    
    // Empfangen
    while(network.available()){
      Payload buffer;
      RF24NetworkHeader header;      
      network.read(header, &buffer, sizeof(buffer)); // Read the incoming data 
      if(header.from_node == LIN_AXIS){
        if(buffer.command == LOCATION){
          sledgePosition = buffer.position;
          Serial.print("Schlitten bei: "); Serial.println(buffer.position);
        } 
        if(buffer.command == DONE){
          Serial.println("Aufgabe erledigt!");
          sledgePosition = buffer.position;
        } 
        #ifdef DEBUG_MODE
          //Serial.print(F("Nachricht von: ")); Serial.println(text[5]);
        #endif
        attempts[5] = 0;  
      }      
    }
    

    // LCD
    TSPoint p = readTouchScreen();

    btBack.create(p);
    btForward.create(p);
    btStop.create(p);
    btPos.create(p);
    btMove.create(p);
    btRev.create(p);
    btReset.create(p);    

    if(sledgePosition != oldPos){
      tft.setTextColor(WHITE, BACKGROUND);
      tft.setCursor(10, 190);
      tft.setTextSize(2);
      tft.print(sledgePosition);
      tft.println("        ");
      oldPos = sledgePosition;
    }
    
    // Senden
    RF24NetworkHeader header(LIN_AXIS);
    Payload package;
    package.command = 0;

    if(btForward.justReleased()) package.command = FORWARD;
    if(btBack.justReleased()) package.command = BACKWARD;
    if(btStop.justReleased()) package.command = STOP;
    if(btMove.justReleased()) package.command = MOVETO;
    if(btPos.justReleased()) package.command = LOCATION;
    if(btRev.justReleased()) package.command = REV;
    if(btReset.justReleased()) package.command = RST;
  
    bool ok = false;
    uint32_t ms = millis();
    while(package.command != 0 && millis() - ms <= 2000){
      network.update();
      ok = network.write(header, &package, sizeof(package));
      if(ok) break;      
    }    

    #ifdef DEBUG_MODE
    if(ok){
      Serial.print(F("Gesendet an: ")); Serial.println(text[5]);      
    }
    #endif    
    
    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }

    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  } 
}

void devMenu(){    
  UI_Keypad pad(&tft, 0, UPPER_BAR + 10, 240, 200, BACKGROUND);

  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);


  //====== Loop =======//
  bool run = true;
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif

    // Netzwerk
    //network.update(); 
    //delay(2);

    // Empfangen
    //receiving();

    // Verbinden
      
    // Senden
    //sending();

    // FPS + Nodes
    if(timerMilli(timeStamp, 250)){
      #ifdef SHOW_FPS
      tft.setCursor(30, 0);
      tft.setTextSize(TEXT_SMALL);
      tft.setTextColor(WHITE, DARKGREY);
      tft.print((uint16_t)(1000000/elapsedTime));
      tft.println("    ");
      #endif

      // Anzahl aktiver Nodes
      uint8_t n = 0;
      for(uint8_t i = 0; i < nodes; i++){
        if(attempts[i] <= ATTEMPTS) n++;
      }
      tft.setTextColor(WHITE, DARKGREY);
      tft.setTextSize(TEXT_NORMAL);
      tft.setCursor(220, 3);
      tft.println(n);
    }

    // LCD
    TSPoint p = readTouchScreen();
    
    pad.create(p);


    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }

    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  }
}

void settingsMenu(){
  UI_Toggle toggle[4];
  #define OFFSET 15

  //====== Setup ======//
  tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
  tft.setTextSize(TEXT_NORMAL);
  tft.setTextColor(WHITE);
  
  tft.setCursor(10, UPPER_BAR + OFFSET);
  tft.println("Einstellungen:");

  //tft.drawFastHLine(0, tft.getCursorY() + OFFSET - 3, 240, DARKGREY);
  //tft.setCursor(10, tft.getCursorY() + OFFSET);
  //toggle[0] = UI_Toggle(&tft, 200, tft.getCursorY(), 40, 21, BACKGROUND);
  //tft.println("Darkmode");

  tft.drawFastHLine(0, tft.getCursorY() + OFFSET - 3, 240, DARKGREY);
  tft.setCursor(10, tft.getCursorY() + OFFSET);
  toggle[1] = UI_Toggle(&tft, 200, tft.getCursorY(), 40, 21, BACKGROUND);
  tft.println("Inaktive Nodes");
  tft.setCursor(10, tft.getCursorY());
  tft.println("anzeigen");
  
  tft.drawFastHLine(0, tft.getCursorY() + OFFSET - 3, 240, DARKGREY);
  tft.setCursor(10, tft.getCursorY() + OFFSET);
  toggle[2] = UI_Toggle(&tft, 200, tft.getCursorY(), 40, 21, BACKGROUND);
  tft.println("Verbinden");

  //tft.drawFastHLine(0, tft.getCursorY() + OFFSET - 3, 240, DARKGREY);
  //tft.setCursor(10, tft.getCursorY() + OFFSET);
  //toggle[3] = UI_Toggle(&tft, 200, tft.getCursorY(), 40, 21, BACKGROUND);
  //tft.println("NRF einschalten");
  //tft.setCursor(10, tft.getCursorY());
  
  toggle[0].setStatus(set.darkmode);
  toggle[1].setStatus(set.showAllNodes);
  toggle[2].setStatus(set.enableConnections);
  toggle[3].setStatus(set.enableNRF);

  //====== Loop =======//
  bool run = true;
  while(run){
    #ifdef SHOW_FPS
    startTime = micros();  
    #endif
    
    // Netzwerk
    network.update(); 
    delay(2);

    // Empfangen
    receiving();

    // Verbinden  
    if(set.enableConnections){
      for(uint8_t i = 0; i < nConnections; i++){
        inputs[con[i].to][con[i].inputPin] = outputs[con[i].from][con[i].outputPin];
      }
    }

    // Senden
    sending(); 

    // FPS + Nodes
    if(timerMilli(timeStamp, 250)){
      #ifdef SHOW_FPS
      tft.setCursor(30, 0);
      tft.setTextSize(TEXT_SMALL);
      tft.setTextColor(WHITE, DARKGREY);
      tft.print((uint16_t)(1000000/elapsedTime));
      tft.println("    ");
      #endif

      // Anzahl aktiver Nodes
      uint8_t n = 0;
      for(uint8_t i = 0; i < nodes; i++){
        if(attempts[i] <= ATTEMPTS) n++;
      }
      tft.setTextColor(WHITE, DARKGREY);
      tft.setTextSize(TEXT_NORMAL);
      tft.setCursor(220, 3);
      tft.println(n);
    }       

    // TouchScreen
    TSPoint p = readTouchScreen();

    for(uint8_t i = 1; i < 3; i++){
      toggle[i].create(p);
    }

    //set.darkmode = toggle[0].getStatus();
    set.showAllNodes = toggle[1].getStatus();
    set.enableConnections = toggle[2].getStatus();
    //set.enableNRF = toggle[3].getStatus();

    
    // Home
    btHome.create(p);
    if(btHome.justReleased()){
      tft.fillRect(0, UPPER_BAR, 240, LOWER_BAR - UPPER_BAR, BLACK);
      btListN.update();
      btP.update();
      btAxis.update();
      btConnect.update();
      btDev.update();
      break;
    }    
    
    #ifdef SHOW_FPS
    elapsedTime = micros() - startTime;
    #endif
  }
}

uint8_t packBools8(bool *b) {
	uint8_t package = 0;
	for (uint8_t i = 0; i < 8; i++) {
		package = package | b[i];
		if (i != 7) package <<= 1;
	}
	return package;
}

void unpackBools8(uint8_t package, bool *b) {
	for (int8_t i = 7; i > -1; i--) {
		b[i] = package & 0x01;
		package >>= 1;
	}
}

void printTouch(TSPoint p){
  if(p.z >= MINPRESSURE && p.z <= MAXPRESSURE){
    Serial.print("X = ");
    Serial.print(p.x);
    Serial.print("   Y = ");
    Serial.print(p.y);
    Serial.print("   Z = ");
    Serial.println(p.z);
  }
}

