/***********************************************************
  cave_01 : logiciel pour l'Arduino de la cave en version 01
***********************************************************/

#include <SoftwareSerial.h>

// Câblage -------------------------------------------------
// LEDs
// LED_BUILTIN 13
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
#define MEASURE_INTERVAL 10000 // One mesure every minute
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
float depthLast6h, deltaLast6h;
float depthLast12h, deltaLast12h;
float depthLast1j, deltaLast1j;
float depthLast7j, deltaLast7j;
float depthLast14j, deltaLast14j;
float depthLast1m, deltaLast1m;
float depthLast3m, deltaLast3m;
float depthLast6m, deltaLast6m;
float depthLast1a, deltaLast1a;
unsigned long timepoint_measure;
unsigned long measures_nb;

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
  depthLast6h = depth;
  depthLast12h = depth;
  depthLast1j = depth;
  depthLast7j = depth;
  depthLast14j = depth;
  depthLast1m = depth;
  depthLast3m = depth;
  depthLast6m = depth;
  depthLast1a = depth;
}
  
void loop()
{
  if (keyPin == HIGH) {
    // Module in AT commands mode

    // Set all the LEDS ON in AT MODE
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(redLedPin, HIGH);
    // Arduino serial monitor is crossed linked to HC-05 data
    // to send AT commands manually to HC-05 from the console
    if (BtSerial.available()) {
      Serial.write(BtSerial.read());
      Serial.println(BtSerial.read());
    }
    if (Serial.available()) {
      BtSerial.write(Serial.read());
      Serial.println(Serial.read());
    }
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
      Serial.print("  delta 6h :");
      Serial.print(deltaLast6h);
      Serial.println("cm");
      Serial.print("  delta 12h :");
      Serial.print(deltaLast12h);
      Serial.println("cm");
      Serial.print("  delta 1j :");
      Serial.print(deltaLast1j);
      Serial.println("cm");
      Serial.print("  delta 7j :");
      Serial.print(deltaLast7j);
      Serial.println("cm");
      Serial.print("  delta 14j :");
      Serial.print(deltaLast14j);
      Serial.println("cm");
      Serial.print("  delta 1m :");
      Serial.print(deltaLast1m);
      Serial.println("cm");
      Serial.print("  delta 3m :");
      Serial.print(deltaLast3m);
      Serial.println("cm");
      Serial.print("  delta 6m :");
      Serial.print(deltaLast6m);
      Serial.println("cm");
      Serial.print("  delta 1a :");
      Serial.print(deltaLast1a);
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
