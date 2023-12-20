// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO
//Technical support:goodtft@163.com
//Modified by Nida @ MYBOTIC www.mybotic.com.my

#include <Elegoo_GFX.h>     // Core graphics library
#include <Elegoo_TFTLCD.h>  // Hardware-specific library
#include <TouchScreen.h>
#include <ArduinoJson.h>

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40
/*
#define YP 9  // must be an analog pin, use "An" notation!
#define XM 8  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A3   // can be a digital pin
*/

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
/*
#define TS_MINX 50
#define TS_MAXX 920

#define TS_MINY 100
#define TS_MAXY 940
*/
//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3

String dataReceived;

String errorLog = "";

void setup(void) {  //body
  log("setup");

  Serial.begin(38400);

  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    // Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    //Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    // Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    // Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    // Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    // Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101) {
    identifier = 0x9341;
    // Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    // Serial.print(F("Unknown LCD driver chip: "));
    // Serial.println(identifier, HEX);
    // Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    // Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    // Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    // Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    // Serial.println(F("Also if using the breakout, double-check that all wiring"));
    //Serial.println(F("matches the tutorial."));
    identifier = 0x9341;
  }


  tft.begin(identifier);
  clearScreen();
  displayButtons();


  // Texte des capteurs
  displayTerminalData("120 ---------\l\nprof. 340cm\l\n  delta 30 mn : -20.00cm\l\n  delta 1h    : -20.00cm\l\n  delta 3h    : -20.00cm\l\l  delta 6h    : -20.00cm");

  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

StaticJsonDocument<200> dataMeasure;

void loop()  //func. definition
{
  // Display Bluetooth received data -----------------------
  if (Serial.available() > 0) {
    // Display the bluetooth received data
    //displayTerminalData(Serial.readString());
    dataReceived = Serial.readString();
    log("data received : " + dataReceived);
    dataReceived.trim();
    // Serial.println(dataReceived);
    displayTerminalData(dataReceived);

    // Affichage en JSON
    DeserializationError error = deserializeJson(dataMeasure, dataReceived);

    // Test if parsing succeeds.
    if (error) {
      log(error.f_str());
      return;
    } else {
      long time = dataMeasure["time"];
      long temp = dataMeasure["temp"];
      log(String(time));
      log(String(temp));
    }
  }

  // Manage Touchscreen --------------------------------------
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    //p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    // Serial.print("X = ");
    // Serial.println(p.x);
    // Serial.print("\tY = ");
    // Serial.println(p.y);
    // Serial.print("\tPressure = ");
    // Serial.println(p.z);
    /*
    tft.fillRect(240, 200, 80, 40, WHITE); // (x,y,widthx,widthy,color)
  // Bouton de gauche
  tft.fillRect(0, 200, 80, 40, WHITE);
  */

    if (p.x < 220) {
      // Serial.println("touche");
      if (p.y < 280) {
        // Serial.println("fct4");
        displayTerminalData(dataReceived);
      } else if (p.y < 490) {
        // Serial.println("fct3");
        if (errorLog.length() == 0) {
          displayTerminalData("Log vide !!!");
        } else {
          displayTerminalData(errorLog);
        }
      } else if (p.y < 710) {
        // Serial.println("fct2");
        structuredDataDisplay();
      } else {
        // Serial.println("fct1");
        displayTerminalData(dataReceived);
      }
    }
  }
}


// ---------------------------------------------------------
// Fonction d'affichage des boutons
//----------------------------------------------------------
void displayButtons() {
  tft.setRotation(1);  // 0 : portrait, 1 : landscape, 2 ou 3 en continuant à tourner
  // 4 boutons de gauche à droite
  tft.fillRect(0, 200, 75, 35, WHITE);  // (x,y,widthx,widthy,color)
  tft.fillRect(80, 200, 75, 35, WHITE);
  tft.fillRect(160, 200, 75, 35, WHITE);
  tft.fillRect(240, 200, 75, 35, WHITE);
  // Légende des touches ----------------
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  // De gauche à droite : Fct1
  tft.setCursor(15, 210);
  tft.println("fct1");
  // fct 2
  tft.setTextSize(2);
  tft.setCursor(82, 210);
  tft.println("format");
  // fct3
  tft.setTextSize(2);
  tft.setCursor(175, 210);
  tft.println("log");
  // fct 4
  tft.setTextSize(1.5);
  tft.setCursor(255, 207);
  tft.println("donnees");
  tft.setCursor(258, 219);
  tft.println("brutes");
}

// ---------------------------------------------------------
// Ecriture des données terminal
//----------------------------------------------------------
void displayTerminalData(String text) {
  clearTerminalZone();
  tft.setRotation(1);  // 0 : portrait, 1 : landscape, 2 ou 3 en continuant à tourner
  tft.setCursor(2, 2);
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.println(text);
}

// ---------------------------------------------------------
// Effacement de la zone hors bouton
//----------------------------------------------------------
void clearTerminalZone() {
  tft.fillRect(0, 0, 320, 200, BLACK);  // (x,y,widthx,widthy,color)
}

// ---------------------------------------------------------
// Effacement de tout l'écran
//----------------------------------------------------------
void clearScreen() {
  tft.fillRect(0, 0, tft.width(), tft.height(), BLACK);  // (x,y,widthx,widthy,color)
}

// ---------------------------------------------------------
// Affichage des données structurées
//----------------------------------------------------------
void structuredDataDisplay() {
  clearTerminalZone();
  tft.setRotation(1);  // 0 : portrait, 1 : landscape, 2 ou 3 en continuant à tourner
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(2, 2);
  tft.println("Temperature");
  tft.setCursor(200, 2);
  long temp = dataMeasure["temp"];
  tft.print(temp);
  tft.println(" C");

  tft.setTextColor(GREEN);
  tft.setCursor(2, 22);
  tft.println("Humidite");
  tft.setCursor(200, 22);
  tft.println("66 %");
  tft.setTextColor(CYAN);
  tft.setCursor(2, 62);
  tft.println("Hauteur d'eau");
  tft.setCursor(200, 62);
  tft.println("335 cm");
  tft.setTextColor(BLUE);
  tft.setCursor(2, 82);
  tft.println("- delta 30 mn");
  tft.setCursor(200, 82);
  tft.println("-10 cm");
  tft.setCursor(2, 102);
  tft.println("- delta 1 h");
  tft.setCursor(200, 102);
  tft.println("-10 cm");
  tft.setCursor(2, 122);
  tft.println("- delta 3 h");
  tft.setCursor(200, 122);
  tft.println("-10 cm");
}

// ---------------------------------------------------------
// Logging
//----------------------------------------------------------
void log(String text) {
  errorLog = errorLog + text + "\n";
}