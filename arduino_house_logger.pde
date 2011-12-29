#include <WProgram.h>
#include <Wire.h>
#include <DS1307.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <FileLogger.h>
#include <avr/interrupt.h>

// define the pin that powers up the SD card
#define MEM_PW 8
#define GREEN_LED 8
#define RED_LED 9

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

#define PIN_ANEMOMETER  2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
DeviceAddress insideThermometer1 = { 0x10, 0x4E, 0xE4, 0x2A, 0x02, 0x08, 0x00, 0x0A };
//DeviceAddress insideThermometer2 = { 0x10, 0x2B, 0xDF, 0x2A, 0x02, 0x08, 0x00, 0xD6 };
//DeviceAddress outsideThermometer = { 0x28, 0x6B, 0xDF, 0xDF, 0x02, 0x00, 0x00, 0xC0 };
//DeviceAddress dogHouseThermometer = { 0x28, 0x59, 0xBE, 0xDF, 0x02, 0x00, 0x00, 0x9F };

int rtc[7];
int compareMinute = -1;

#define SECOND 	 0
#define MINUTE 	 1
#define HOUR 	 2
#define DAYOFWEEK 	 3
#define DAYOFMONTH 	 4
#define MONTH 	 5
#define YEAR 	 6

volatile long count = 0;

void setup()
{  
  pinMode(MEM_PW, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  digitalWrite(MEM_PW, HIGH);
  
  Serial.begin(9600);
  
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(insideThermometer1, 10);
  //sensors.setResolution(insideThermometer2, 10);
  //sensors.setResolution(outsideThermometer, 10);
  //sensors.setResolution(dogHouseThermometer, 10);
   
  pinMode(PIN_ANEMOMETER, INPUT);
  digitalWrite(PIN_ANEMOMETER, HIGH);
  attachInterrupt(0, countAnemometer, FALLING);

  /*
  //to set clock
  RTC.stop();

  RTC.set(DS1307_MTH, 11);
  RTC.set(DS1307_DATE, 14);
  RTC.set(DS1307_YR,  11);

  RTC.set(DS1307_DOW, 2); //1=sunday?

  RTC.set(DS1307_HR, 20);
  RTC.set(DS1307_MIN, 2);
  RTC.set(DS1307_SEC, 0);
  
  RTC.start();
  */
  
  log("DateTime\tF");
}



String toString(int i) {
  if (i < 10)
    return "0" + String(i);
  else
    return String(i);
}

String getTime() {
  RTC.get(rtc,true);

  int second = rtc[0];
  int minute = rtc[1];
  int hour = rtc[2];
  int dayOfWeek = rtc[3];
  int dayOfMonth = rtc[4];
  int month = rtc[5];
  int year = rtc[6];

  return String(year) + "-" + toString(month) + "-" + toString(dayOfMonth) + " " + toString(hour) + ":" + toString(minute) + ":" + toString(second);
}

String getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  //if (tempC == -127.00) {
  //  Serial.print("Error getting temperature");   
    
  //char ch[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  //char *c = ch;
  //dtostrf(tempC, 10, 4, ch);
  //while (*c == ' ')
  //  *c++;
  //String tempCString = String(c); 


  char fh[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  char *f = fh;
  dtostrf(DallasTemperature::toFahrenheit(tempC), 10, 4, fh);
  while (*f == ' ')
    *f++;
  String tempFString = String(f);
  
  return tempFString;
}

int log(String s) {
  byte buffer[100];
  int i = 0;
  for (i = 0; i < s.length() && i < 100; i++) {
    buffer[i] = s.charAt(i);
  }
  buffer[i++] = '\r';
  buffer[i++] = '\n';
  buffer[i] = '\0';
  
  return FileLogger::append("data.txt", buffer, i);
}

void countAnemometer() {
  count++;
}

void loop()
{
  String timeString = getTime();
  
  if (rtc[MINUTE] > compareMinute || rtc[MINUTE] == 0 && compareMinute == 59) {
    compareMinute = rtc[MINUTE];
    sensors.requestTemperatures();  
    digitalWrite(GREEN_LED, HIGH);
    
    //Serial.print("Inside temperature1 is: ");
    String tempString = getTemperature(insideThermometer1);
    
    String out = timeString + "\t" + tempString "\t" + String(count);
    count = 0;
    
    //Serial.print(timeString);
    //Serial.print("|");
    //Serial.print(tempString);  
    //Serial.print("\n");
    
    Serial.println(out);
    int logResult = log(out);
    if (logResult != 0) {
          digitalWrite(RED_LED, HIGH);
          delay(1000);
          digitalWrite(RED_LED, LOW);
    }
    Serial.println(log(out));

    digitalWrite(GREEN_LED, LOW);
  }
  
  delay(5000);
}
