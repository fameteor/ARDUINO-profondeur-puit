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
#define BtTxPin 8  // to HC-05 RX pin
#define BtRxPin 9  // to HC-05 TX pin
// Depth mesurement ---------------------------
#define ANALOG_PIN A3

// Measure interval ---------------------------
#define MEASURE_INTERVAL 6000  // Ten measures every minute
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
int humidity;
int temperature;
// Depth variables ----------------------------
float depth;
float totalForMeanCalculus = 0;
float depthLast30mn = 0;
float depthLast1h = 0;
float depthLast3h = 0;
float depthLast6h = 0;
float depthLast12h = 0;
float depthLast1j = 0;
float depthLast7j = 0;
float depthLast14j = 0;
float depthLast1m = 0;
float depthLast3m = 0;
float depthLast6m = 0;
float depthLast1a = 0;

float deltaLast30mn = 0;
float deltaLast1h = 0;
float deltaLast3h = 0;
float deltaLast6h = 0;
float deltaLast12h = 0;
float deltaLast1j = 0;
float deltaLast7j = 0;
float deltaLast14j = 0;
float deltaLast1m = 0;
float deltaLast3m = 0;
float deltaLast6m = 0;
float deltaLast1a = 0;

String jsonData;


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

  jsonData.reserve(100);

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
    dataFile.print("On ");
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
    // We sum the depth measures 10 times for mean calculus
    totalForMeanCalculus += getDepth();
    // If 10 measures, we calculate the means value
    if (measures_nb % 10 == 9) {
      // Get the measures
      now = RTC.now();
      depth = totalForMeanCalculus / 10;
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();

      Serial.print("Mean depth : ");
      Serial.println(depth);
      // We reset the mean accumulator
      totalForMeanCalculus = 0;
      // We initialise the deltas if the first measure (index == 0)
      if (measures_nb / 10 == 0) {
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
        // Serial.println("depth initialised for deltas");
      } else {
        // We calculate the deltas
        calculateDeltas();
      }
      // We get the measure JSON values
      getJSONdata();
      // We send it over Bluetooth
      BtSerial.print(jsonData);
      // We write it to SD card
      // Open file
      dataFile = SD.open("cave.txt", FILE_WRITE);
      if (dataFile) {
        // If file available, write to it
        dataFile.println(jsonData);
        dataFile.close();
        // print to the serial port too:
        Serial.println("data stored");
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening cave.txt");
      }
    }
    // In any case
    measures_nb += 1;
  }
}

// ---------------------------------------------------------
// Get JSON data string
//----------------------------------------------------------
void getJSONdata() {
  // Format : {"n":525600,"d":[2023,12,25,12,45,13],"t":30,"x":334,"h":66,"v":[-1,-2,-3,-4]};
  // Uses global variables : now, measures_nb, temperature, depth, humidity and deltas
  jsonData = "{\"n\":";
  jsonData += String(measures_nb / 10);
  jsonData += ",\"d\":[";
  jsonData += String(now.year(), DEC);
  jsonData += (",");
  jsonData += String(now.month(), DEC);
  jsonData += (",");
  jsonData += String(now.day(), DEC);
  jsonData += (",");
  jsonData += String(now.hour(), DEC);
  jsonData += (",");
  jsonData += String(now.minute(), DEC);
  jsonData += (",");
  jsonData += String(now.second(), DEC);
  jsonData += ("]");
  jsonData += (",\"t\":");
  jsonData += String(temperature);
  jsonData += (",\"x\":");
  jsonData += String(depth, 0);
  jsonData += (",\"h\":");
  jsonData += String(humidity);
  jsonData += (",\"v\":[");
  jsonData += String(deltaLast30mn, 0);
  jsonData += (",");
  jsonData += String(deltaLast1h, 0);
  jsonData += (",");
  jsonData += String(deltaLast3h, 0);
  jsonData += (",");
  jsonData += String(deltaLast6h, 0);
  jsonData += ("]}");
  Serial.println(jsonData);
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
}

// ---------------------------------------------------------
// Calculate Deltas
//----------------------------------------------------------
void calculateDeltas() {
  if ((measures_nb/10) % 30 == 0) {
    // Serial.println("demi-heure");
    deltaLast30mn = depth - depthLast30mn;
    depthLast30mn = depth;
  }
  if ((measures_nb/10) % 60 == 0) {
    // Serial.println("heure");
    deltaLast1h = depth - depthLast1h;
    depthLast1h = depth;
  }
  if ((measures_nb/10) % 180 == 0) {
    // Serial.println("3 heures");
    deltaLast3h = depth - depthLast3h;
    depthLast3h = depth;
  }
  if ((measures_nb/10) % 360 == 0) {
    // Serial.println("6 heures");
    deltaLast6h = depth - depthLast6h;
    depthLast6h = depth;
  }
  if ((measures_nb/10) % 720 == 0) {
    // Serial.println("12 heures");
    deltaLast12h = depth - depthLast12h;
    depthLast12h = depth;
  }
  if ((measures_nb/10) % 1440 == 0) {
    // Serial.println("1 jour");
    deltaLast1j = depth - depthLast1j;
    depthLast1j = depth;
  }
  if ((measures_nb/10) % 10080 == 0) {
    // Serial.println("7 jours");
    deltaLast7j = depth - depthLast7j;
    depthLast7j = depth;
  }
  if ((measures_nb/10) % 20160 == 0) {
    // Serial.println("14 jours");
    deltaLast14j = depth - depthLast14j;
    depthLast14j = depth;
  }
  if ((measures_nb/10) % 43920 == 0) {
    // Serial.println("1 mois") // 30,5 jours par mois ;
    deltaLast1m = depth - depthLast1m;
    depthLast1m = depth;
  }
  if ((measures_nb/10) % 131760 == 0) {
    // Serial.println("3 mois") // 30,5 jours par mois ;
    deltaLast3m = depth - depthLast3m;
    depthLast3m = depth;
  }
  if ((measures_nb/10) % 263520 == 0) {
    // Serial.println("6 mois") // 30,5 jours par mois;
    deltaLast6m = depth - depthLast6m;
    depthLast6m = depth;
  }
  if ((measures_nb/10) % 525600 == 0) {
    // Serial.println("1 an") // 365 jours par an;
    deltaLast1a = depth - depthLast1a;
    depthLast1a = depth;
    measures_nb = 0;
  }
}