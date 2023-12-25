#include <SD.h>
#include <Wire.h>  // I2C bus management
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <DHT.h>

// --------------------------------------------
// PINS
// --------------------------------------------

// Temperature and humidity measurement -------
#define DHTpin 7
#define DHTtype DHT11
// Bluetooth module ---------------------------
#define BtRxPin 8  // to HC-05 RX pin   // 10 previously
#define BtTxPin 9  // to HC-05 TX pin   // 11 previously
// Depth mesurement ---------------------------
#define ANALOG_PIN A3

// Measure interval ---------------------------
#define MEASURE_INTERVAL 5000  // One mesure every minute
// Depth calculus -----------------------------
#define RANGE 5000         // Depth measuring range 5000mm (for water)
#define VREF 5000          // ADC's reference voltage on your Arduino,typical value:5000mV
#define CURRENT_INIT 4.00  // Current @ 0mm (uint: mA)
#define DENSITY_WATER 1    // Pure water density normalized to 1

// SD shiel management ------------------------
const int chipSelect = 10;  //cs or the save select pin from the sd shield is connected to 10.
// RTC management -----------------------------
RTC_DS1307 RTC;
// Measures control ---------------------------
unsigned long timepoint_measure;
unsigned long measures_nb;
// File variable ------------------------------
File dataFile;
// DateTime variable --------------------------
DateTime now;
// Calculus variables -------------------------
float humidity;
float temperature;
// Deltas variables ---------------------------
float depthLast30mn, deltaLast30mn;
float depthLast1h, deltaLast1h;
float depthLast3h, deltaLast3h;
float depthLast6h, deltaLast6h;
float depthLast12h, deltaLast12h;
float depthLast1j, deltaLast1j;
float depthLast7j, deltaLast7j;
float depthLast14j, deltaLast14j;
float depthLast1m, deltaLast1m;
float depthLast3m, deltaLast3m;
float depthLast6m, deltaLast6m;
float depthLast1a, deltaLast1a;

// Bluetooth serial link creation -------------
SoftwareSerial BtSerial(BtRxPin, BtTxPin);  // RX | TX
// DHT initialisation -------------------------
DHT dht(DHTpin, DHTtype);

// --------------------------------------------
// SETUP
// --------------------------------------------
void setup(void) {
  // Measures control initialisation ---------
  timepoint_measure = millis();
  measures_nb = 0;
  // Deltas initialisation
  float depth = getDepth();
  depthLast30mn = depth;
  depthLast1h = depth;
  depthLast3h = depth;
  depthLast6h = depth;
  depthLast12h = depth;
  depthLast1j = depth;
  depthLast7j = depth;
  depthLast14j = depth;
  depthLast1m = depth;
  depthLast3m = depth;
  depthLast6m = depth;
  depthLast1a = depth;

  // DHC11 initialisation --------------------
  dht.begin();
  // Serial ports are configured to 38400 bps : HC-05 default speed in AT command mode
  Serial.begin(38400);
  BtSerial.begin(38400);
  // Depth sensor initialisation -------------
  pinMode(ANALOG_PIN, INPUT);
  // Clock setup -----------------------------
  Wire.begin();
  RTC.begin();
  //check or the Real Time Clock is on
  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  // following line sets the RTC to the date & time this sketch was compiled
  // uncomment it & upload to set the time, date and start run the RTC!
  // RTC.adjust(DateTime(2023, 12, 22, 14, 22, 0));
  //setup SD card
  Serial.print("Initializing SD card...");

  // see if the SD card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  //write down the date (year / month / day         prints only the start, so if the logger runs for sevenal days you only findt the start back at the begin.
  now = RTC.now();
  dataFile = SD.open("cave.txt", FILE_WRITE);
  if (dataFile) {
    // If file available, write to it
    dataFile.print("Power on ");
    dataFile.print(now.day(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.year(), DEC);
    dataFile.print(" ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.println(now.second(), DEC);
    dataFile.close();
    // print to the serial port too:
    Serial.println("startup stored");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening cave.txt");
  }
}


// --------------------------------------------
// LOOP
// --------------------------------------------
void loop(void) {
  // Mesure
  if (millis() - timepoint_measure > MEASURE_INTERVAL) {
    // Manage next measure
    timepoint_measure = millis();
    String measure = getJSONdata();
    measures_nb += 1;
    // Open file
    dataFile = SD.open("cave.txt", FILE_WRITE);
    if (dataFile) {
      // If file available, write to it
      dataFile.println(measure);
      dataFile.close();
      // print to the serial port too:
      Serial.println("data stored");
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening cave.txt");
    }
    // Send to Bluetooth
    BtSerial.print(measure);
  }
}

// ---------------------------------------------------------
// Get JSON data string
//----------------------------------------------------------
String getJSONdata() {
  DateTime now = RTC.now();
  String JSONdata = "{\"n\":";
  JSONdata = JSONdata + String(measures_nb);
  JSONdata = JSONdata + ",\"d\":[";
  // Format : {"n":525600,"d":[2023,12,25,12,45,13],"t":30,"x":334,"h":66,"v":[-1,-2,-3,-4]};
  JSONdata = JSONdata + String(now.year(), DEC);
  JSONdata = JSONdata + (",");
  JSONdata = JSONdata + String(now.month(), DEC);
  JSONdata = JSONdata + (",");
  JSONdata = JSONdata + String(now.day(), DEC);
  JSONdata = JSONdata + (",");
  JSONdata = JSONdata + String(now.hour(), DEC);
  JSONdata = JSONdata + (",");
  JSONdata = JSONdata + String(now.minute(), DEC);
  JSONdata = JSONdata + (",");
  JSONdata = JSONdata + String(now.second(), DEC);
  JSONdata = JSONdata + ("]");
  JSONdata = JSONdata + (",\"t\":");
  JSONdata = JSONdata + String(getTemp());
  JSONdata = JSONdata + (",\"x\":");

  float depth = getDepth();
  // calculateDeltas(depth);
  JSONdata = JSONdata + String(depth, 0);
  JSONdata = JSONdata + (",\"h\":");
  JSONdata = JSONdata + String(getHumidity());
  JSONdata = JSONdata + (",\"v\":[-1,-2,-3,-4");
  JSONdata = JSONdata + ("]}");
  Serial.println(JSONdata);
  return JSONdata;
}

// ---------------------------------------------------------
// Get cellar temperature
//----------------------------------------------------------
int getTemp() {
  return dht.readTemperature();
}

// ---------------------------------------------------------
// Get cellar humidity
//----------------------------------------------------------
int getHumidity() {
  return dht.readHumidity();
}

// ---------------------------------------------------------
// Get water depth
//----------------------------------------------------------
float getDepth() {
  int16_t dataVoltage;
  float dataCurrent, depthMesure;  //mA for current
  dataVoltage = analogRead(ANALOG_PIN) / 1024.0 * VREF;
  dataCurrent = dataVoltage / 120.0;                                                 //Sense Resistor:120ohm
  depthMesure = (dataCurrent - CURRENT_INIT) * (RANGE / DENSITY_WATER / 16.0 / 10);  //Calculate depth from current readings in cm
  //  if (depthMesure < 0) depthMesure = 0.0;

  // Delta calculus ----------------------------------------
  return depthMesure;
  // return 335.25;
}

// ---------------------------------------------------------
// Calculate Deltas
//----------------------------------------------------------
void calculateDeltas(float depth) {
  if (measures_nb % 30 == 0) {
    // Serial.println("demi-heure");
    deltaLast30mn = depth - depthLast30mn;
    depthLast30mn = depth;
  }
  if (measures_nb % 60 == 0) {
    // Serial.println("heure");
    deltaLast1h = depth - depthLast1h;
    depthLast1h = depth;
  }
  if (measures_nb % 180 == 0) {
    // Serial.println("3 heures");
    deltaLast3h = depth - depthLast3h;
    depthLast3h = depth;
  }
  if (measures_nb % 360 == 0) {
    // Serial.println("6 heures");
    deltaLast6h = depth - depthLast6h;
    depthLast6h = depth;
  }
  if (measures_nb % 720 == 0) {
    // Serial.println("12 heures");
    deltaLast12h = depth - depthLast12h;
    depthLast12h = depth;
  }
  if (measures_nb % 1440 == 0) {
    // Serial.println("1 jour");
    deltaLast1j = depth - depthLast1j;
    depthLast1j = depth;
  }
  if (measures_nb % 10080 == 0) {
    // Serial.println("7 jours");
    deltaLast7j = depth - depthLast7j;
    depthLast7j = depth;
  }
  if (measures_nb % 20160 == 0) {
    // Serial.println("14 jours");
    deltaLast14j = depth - depthLast14j;
    depthLast14j = depth;
  }
  if (measures_nb % 43920 == 0) {
    // Serial.println("1 mois") // 30,5 jours par mois ;
    deltaLast1m = depth - depthLast1m;
    depthLast1m = depth;
  }
  if (measures_nb % 131760 == 0) {
    // Serial.println("3 mois") // 30,5 jours par mois ;
    deltaLast3m = depth - depthLast3m;
    depthLast3m = depth;
  }
  if (measures_nb % 263520 == 0) {
    // Serial.println("6 mois") // 30,5 jours par mois;
    deltaLast6m = depth - depthLast6m;
    depthLast6m = depth;
  }
  if (measures_nb % 525600 == 0) {
    // Serial.println("1 an") // 365 jours par an;
    deltaLast1a = depth - depthLast1a;
    depthLast1a = depth;
    measures_nb = 0;
  }
}