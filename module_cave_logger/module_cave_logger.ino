
#include <OneWire.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
const int chipSelect = 10;  //cs or the save select pin from the sd shield is connected to 10.
RTC_DS1307 RTC;
float celsius, fahrenheit;

OneWire ds(8);  // temperature senor on pin 8 (a 4.7K resistor is necessary) //

File dataFile;
DateTime now;

void setup(void) {
  Serial.begin(9600);
  //setup clock
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
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile.print("Power on ");
  dataFile.print(now.year(), DEC);
  dataFile.print('/');
  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  dataFile.print(now.day(), DEC);
  dataFile.print(" ");
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.println(now.second(), DEC);
  dataFile.close();
}

void loop(void) {

  //read the time
  now = RTC.now();

  //open file to log data in.
  dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  // log the temperature and time.
  if (dataFile) {
    dataFile.print("{\"Y\":");
    dataFile.print(now.year(), DEC);
    dataFile.print(",\"M\":");
    dataFile.print(now.month(), DEC);
    dataFile.print(",\"D\":");
    dataFile.print(now.day(), DEC);
    dataFile.print(",\"H\":");
    dataFile.print(now.hour(), DEC);
    dataFile.print(",\"M\":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(",\"S\":");
    dataFile.print(now.second(), DEC);
    dataFile.println(",\"temp\":22}");
    dataFile.close();
    // print to the serial port too:
    Serial.println("data stored");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  //delay(60000); // this will log the temperature every minute.
  delay(3000);
}