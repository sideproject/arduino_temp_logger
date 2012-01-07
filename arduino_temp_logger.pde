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

//#define PIN_ANEMOMETER  2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
//DeviceAddress insideThermometer1 = { 0x10, 0x4E, 0xE4, 0x2A, 0x02, 0x08, 0x00, 0x0A };
//DeviceAddress insideThermometer2 = { 0x10, 0x2B, 0xDF, 0x2A, 0x02, 0x08, 0x00, 0xD6 };
//DeviceAddress outsideThermometer = { 0x28, 0x6B, 0xDF, 0xDF, 0x02, 0x00, 0x00, 0xC0 };
//DeviceAddress dogHouseThermometer = { 0x28, 0x59, 0xBE, 0xDF, 0x02, 0x00, 0x00, 0x9F };

DeviceAddress officeThermometer = { 0x10, 0xD5, 0xDC, 0x2A, 0x02, 0x08, 0x00, 0xF9 };
DeviceAddress bedroomThermometer = { 0x10, 0x4E, 0xE4, 0x2A, 0x02, 0x08, 0x00, 0x0A };
DeviceAddress livingroomThermometer = { 0x10, 0x2B, 0xDF, 0x2A, 0x02, 0x08, 0x00, 0xD6 };

int rtc[7];
int lastRunMinute = -1;

#define SECOND 	 0
#define MINUTE 	 1
#define HOUR 	 2
#define DAYOFWEEK 	 3
#define DAYOFMONTH 	 4
#define MONTH 	 5
#define YEAR 	 6

//volatile long count = 0;

int lastRunIsError = 0;

void setup()
{
  lastRunIsError = 0;
  lastRunMinute = -1;
  
  pinMode(MEM_PW, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  digitalWrite(MEM_PW, HIGH);
  
  Serial.begin(9600);
  
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(officeThermometer, 10);
  sensors.setResolution(bedroomThermometer, 10);
  sensors.setResolution(livingroomThermometer, 10);
  //sensors.setResolution(insideThermometer2, 10);
  //sensors.setResolution(outsideThermometer, 10);
  //sensors.setResolution(dogHouseThermometer, 10);
   
  //pinMode(PIN_ANEMOMETER, INPUT);
  //digitalWrite(PIN_ANEMOMETER, HIGH);
  //attachInterrupt(0, countAnemometer, FALLING);

/*
  //to set clock
  RTC.stop();

  RTC.set(DS1307_MTH, 12);
  RTC.set(DS1307_DATE, 27);
  RTC.set(DS1307_YR,  11);

  RTC.set(DS1307_DOW, 3); //1=sunday?

  RTC.set(DS1307_HR, 14);
  RTC.set(DS1307_MIN, 55);
  RTC.set(DS1307_SEC, 0);
  
  RTC.start();
*/
  
  //log("DateTime\tF");
  
  Serial.println("RESET");
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
  //int dayOfWeek = rtc[3];
  int dayOfMonth = rtc[4];
  int month = rtc[5];
  int year = rtc[6];
  
  //char returner[25] = "yyyy-mm-dd hh:mm:ss";
  
  //int value1 = 0;     // variable to read the value from the analog pin 0 
  //char value2[4] ; 
  //itoa (value1, value2, 10);
  
   char cYear[5]="";
   itoa(year, cYear,10);
   char cMonth[3]="";
   itoa(month, cMonth,10);
   char cDay[3]="";
   itoa(dayOfMonth, cDay,10); 
   char cHour[3]="";
   itoa(hour, cHour,10);
   char cMinute[3]="";
   itoa(minute, cMinute,10);
   char cSecond[3]="";
   itoa(second, cSecond,10);
   
   if (month < 10) {
     cMonth[1] = cMonth[0];
     cMonth[0] = '0';
   }
   if (dayOfMonth < 10) {
     cDay[1] = cDay[0];
     cDay[0] = '0';
   }
   if (hour < 10) {
     cHour[1] = cHour[0];
     cHour[0] = '0';
   }
   if (minute < 10) {
     cMinute[1] = cMinute[0];
     cMinute[0] = '0';
   }
   if (second < 10) {
     cSecond[1] = cSecond[0];
     cSecond[0] = '0';
   }
   char returner[25] = "yyyy-mm-dd hh:mm:ss";
   returner[0] = cYear[0];
   returner[1] = cYear[1];
   returner[2] = cYear[2];
   returner[3] = cYear[3];
   returner[4] = '-';
   returner[5] = cMonth[0];
   returner[6] = cMonth[1];
   returner[7] = '-';
   returner[8] = cDay[0];
   returner[9] = cDay[1];
   returner[10]= ' ';
   returner[11]= cHour[0];
   returner[12]= cHour[1];
   returner[13]= ':';
   returner[14]= cMinute[0];
   returner[15]= cMinute[1];
   returner[16]= ':';
   returner[17]= cSecond[0];
   returner[18]= cSecond[1];
   returner[19]= '\0';
//   Serial.println(returner);   
   return String(returner);
   
  //return String(year) + "-" + toString(month);// + "-" + toString(dayOfMonth) + " " + toString(hour) + ":" + toString(minute) + ":" + toString(second);
}

float getTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  return DallasTemperature::toFahrenheit(tempC);
}

String getTemperatureString(float tempF)
{
  char fh[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  char *f = fh;
  dtostrf(tempF, 10, 2, fh);
  while (*f == ' ')
    *f++;
  return String(f);
}

/*
int log(String s) {
  byte buffer[100];
  int i = 0;
  for (i = 0; i < 100; i++) {
    buffer[i] = '\0';
  }
  for (i = 0; i < s.length() && i < 99; i++) {
    buffer[i] = s.charAt(i);
  }
  return FileLogger::append("data.txt", buffer, i);
}*/

int logLine(String s) {
  Serial.println("Logging: " + s);
  byte buffer[100];
  int i = 0;
  for (i = 0; i < 100; i++) {
    buffer[i] = '\0';
  }
  for (i = 0; i < s.length() && i < 95; i++) {
    buffer[i] = s.charAt(i);
  }
  buffer[i++] = '\r';
  buffer[i++] = '\n';
  buffer[i] = '\0';  
  return FileLogger::append("data.txt", buffer, i);
}

//void countAnemometer() {
//  count++;
//}

void blink(int pin, int ms) {
 digitalWrite(pin, HIGH);
  delay(ms);
  digitalWrite(pin, LOW);
  delay(ms);
 
}

void loop()
{
 //Serial.println("LOOP-START");
  String timeString = getTime() + "\t";
  //Serial.println(timeString);
  //Serial.println("TT");
  if (rtc[MINUTE] != lastRunMinute) {
    lastRunIsError = 0;
    Serial.println("Checking");
 
    lastRunMinute = rtc[MINUTE];
    sensors.requestTemperatures();  
    blink(GREEN_LED, 500);
    blink(GREEN_LED, 500);
    blink(GREEN_LED, 500);
    blink(GREEN_LED, 500);
    
    float tempOffice = getTemperature(officeThermometer);
    float tempBedroom = getTemperature(bedroomThermometer);
    float tempLivingroom = getTemperature(livingroomThermometer);
    
    String tempStringOffice = getTemperatureString(tempOffice) + "\t";
    String tempStringBedroom = getTemperatureString(tempBedroom) + "\t";
    String tempStringLivingroom = getTemperatureString(tempLivingroom) + "\t";
     
    //Serial.print(timeString);
    //Serial.print(tempStringOffice);
    //Serial.print(tempStringBedroom);
    //Serial.println(tempStringLivingroom);

    int tempErrors = 0;
    if (tempOffice > 100 || tempOffice < -100)
      tempErrors++;
    if (tempBedroom > 100 || tempBedroom < -100)
      tempErrors++;
    if (tempLivingroom > 100 || tempLivingroom < -100)
      tempErrors++;
    //Serial.println("BEFORELOG");
    int logResult = 0;
    String x = timeString + tempStringOffice + tempStringBedroom + tempStringLivingroom;
    //Serial.println(x.length());
    logResult += logLine(x);
    //logResult += logLine("ABC");
    /*logResult += log(timeString);
    delay(400);
    logResult += log(tempStringOffice);
    delay(400);
    logResult += log(tempStringBedroom);
    delay(400);
    logResult += logLine(tempStringLivingroom);
    delay(400);*/
    //Serial.println("AFTERLOG");

    if (logResult > 0) {
        lastRunIsError = 1;
        digitalWrite(RED_LED, HIGH);
        Serial.println("Error Logging");
        Serial.println(logResult);
        delay(1000);
        digitalWrite(RED_LED, LOW);
    }
	
    if (tempErrors > 1) {
        lastRunIsError = 1;
        Serial.println("Error Temps");
        Serial.println(tempErrors);
        delay(1000);
        blink(RED_LED, 100);
        blink(RED_LED, 100);
        delay(1000);
    }
  }

  delay(5000);
  
  blink(GREEN_LED, 100);
 
  if (lastRunIsError)
    blink(RED_LED, 100);
  
 blink(GREEN_LED, 100);
 if (lastRunIsError)
  blink(RED_LED, 100);
 // Serial.println("LOOP-END");
}
