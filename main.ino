#include "SD.h"
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

File myFile;
int RXPin = 6;
int TXPin = 7;

TinyGPSPlus gps;
SoftwareSerial SerialGPS(RXPin, TXPin);

String Latitude, Longitude, Altitude, Speed, day, month, year, hour, minute, second, Date, Time, Data;

void setup() {
  Serial.begin(9600);
  SerialGPS.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("Creating GPS_data.txt...");
  myFile = SD.open("GPS_data.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("Latitude, Longitude, Altitude, Speed, Time \r\n");
    myFile.close();
  } else {
    Serial.println("error opening GPS_data.txt");
  }
  SerialGPS.begin(9600);
}

void loop() {
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
      obtain_data();
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("GPS NOT DETECTED!");
    while (true);
  }
}

void obtain_data() {
  if (gps.location.isValid()) {
    Latitude = String(gps.location.lat(), 6);
    Longitude = String(gps.location.lng(), 6);
    Altitude = String(gps.altitude.meters(), 2);
    Speed = String(gps.speed.kmph(), 2);
  } else {
    Serial.println("Location is not available");
    return;
  }

  Serial.print("UTC Time: ");
  if (gps.time.isValid()) {
    int utcHour = gps.time.hour();
    int utcMinute = gps.time.minute();
    int utcSecond = gps.time.second();

    // Adjust for local time (+5:45)
    int localHour = (utcHour + 5) % 24;
    int localMinute = (utcMinute + 45) % 60;
    int localSecond = utcSecond;

    hour = localHour < 10 ? "0" + String(localHour) : String(localHour);
    minute = localMinute < 10 ? "0" + String(localMinute) : String(localMinute);
    second = localSecond < 10 ? "0" + String(localSecond) : String(localSecond);

    Time = hour + ":" + minute + ":" + second;
    Serial.println(Time);
  } else {
    Serial.print("Time Not Available");
    return;
  }

  Data = Latitude + "," + Longitude + "," + Altitude + ',' + Speed + "," + Time;
  Serial.print("Save data: ");
  Serial.println(Data);

  myFile = SD.open("GPS_data.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("GPS logging to GPS_data.txt...");
    myFile.println(Data);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening GPS_data.txt");
  }

  Serial.println();
  delay(10000);
}
