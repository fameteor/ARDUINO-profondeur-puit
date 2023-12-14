/***********************************************************
  cave_01 : logiciel pour l'Arduino de la cave en version 01
***********************************************************/

#include <SoftwareSerial.h>

// Câblage -------------------------------------------------
// LEDs
#define  greenLedPin 6
#define  yellowLedPin 7
#define  redLedPin 8
// Bluetooth
#define  BtKeyPin 9 // to HC-05 key pin
#define  BtRxPin 10 // to HC-05 RX pin
#define  BtTxPin 11 // to HC-05 TX pin
// Mesure
#define ANALOG_PIN A2

// Measure interval ----------------------------------------
#define MEASURE_INTERVAL 1000 // One mesure every minute
// Calculus ------------------------------------------------
#define RANGE 5000 // Depth measuring range 5000mm (for water)
#define VREF 5000 // ADC's reference voltage on your Arduino,typical value:5000mV
#define CURRENT_INIT 4.00 // Current @ 0mm (uint: mA)
#define DENSITY_WATER 1  // Pure water density normalized to 1

// Variables -----------------------------------------------
int keyPin;
int commandCode;
float depth;
float depthLast30mn, deltaLast30mn;
float depthLast1h, deltaLast1h;
float depthLast3h, deltaLast3h;
unsigned long timepoint_measure;
int measures_nb;

// Bluetooth serial link creation --------------------------
SoftwareSerial BtSerial(10, 11); // RX | TX
  
void setup()
{
  pinMode (greenLedPin, OUTPUT);
  pinMode (yellowLedPin, OUTPUT);
  pinMode (redLedPin, OUTPUT);
  pinMode(BtKeyPin, INPUT);
  pinMode (BtRxPin, INPUT);
  pinMode (BtTxPin, OUTPUT);

  // Serial ports are configured to 38400 bps : HC-05 default speed in AT command mode
  Serial.begin(38400);
  BtSerial.begin(38400);  
  // We read the HC-05 mode : DATA or AT commands
  keyPin = digitalRead (BtKeyPin);
  
  if (keyPin == HIGH) {
    Serial.println("Module in AT commands mode, please enter AT command");
  }
  else {
    Serial.println("Module in DATA mode");
  }

  // Measure
  pinMode(ANALOG_PIN, INPUT);
  timepoint_measure = millis();
  measures_nb = 0;
  // Measures initialisation
  depth = getDepth();
  depthLast30mn = depth;
  depthLast1h = depth;
  depthLast3h = depth;
}
  
void loop()
{
  if (keyPin == HIGH) {
    // Module in AT commands mode
    // Arduino serial monitor is crossed linked to HC-05 data
    /*
    if (BtSerial.available()) Serial.write(BtSerial.read());
    if (Serial.available())   BtSerial.write(Serial.read());
    */
  }
  else {
    // Module in DATA mode" --------------------------
    // Mesure
    if (millis() - timepoint_measure > MEASURE_INTERVAL) {
      timepoint_measure = millis();
      depth = getDepth();
      measures_nb += 1;
      //Serial print results
      Serial.print(measures_nb);
      Serial.print("-depth:");
      Serial.print(depth);
      Serial.println("cm");

      // Initial measure for delta calculus;
      
      // Deltas calculus
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
        measures_nb = 0;
      }    
      // Print deltas
      Serial.print("  delta 30mn :");
      Serial.print(deltaLast30mn);
      Serial.println("cm");
      Serial.print("  delta 1h :");
      Serial.print(deltaLast1h);
      Serial.println("cm");
      Serial.print("  delta 3h :");
      Serial.print(deltaLast3h);
      Serial.println("cm");

    }
    // Envoi de la mesure en Bluetooth
    BtSerial.println(depth);

    // Réception d'action
 
    if (BtSerial.available()) {
      /* 
      commandCode = BtSerial.read();
      Serial.print("commande recue : ");
      Serial.println(commandCode);
      
      // Define actions for command code :
      if (commandCode == '1') {digitalWrite(12, HIGH);}
      if (commandCode == '0') {digitalWrite(12, LOW);}
      if (commandCode == '2') {digitalWrite(10, HIGH);}
      if (commandCode == '3') {digitalWrite(10, LOW);}
      */
    }
    
  }
  
}

float getDepth() {
  int16_t dataVoltage;
  float dataCurrent, depthMesure; //mA for current
  dataVoltage = analogRead(ANALOG_PIN)/ 1024.0 * VREF;
  dataCurrent = dataVoltage / 120.0; //Sense Resistor:120ohm
  depthMesure = (dataCurrent - CURRENT_INIT) * (RANGE/ DENSITY_WATER / 16.0 / 10); //Calculate depth from current readings in cm
  //  if (depthMesure < 0) depthMesure = 0.0;
  return depthMesure;
}