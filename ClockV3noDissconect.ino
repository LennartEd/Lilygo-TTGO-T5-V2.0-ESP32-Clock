#define LILYGO_T5_V213

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <string.h>

#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <time.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);

GxEPD_Class display(io, /*RST=*/ EPD_RSET, /*BUSY=*/ EPD_BUSY); // default selection of (9), 7

//wifi
const char* ssid = "o2-WLAN35-2,4";
const char* password = "2591126232129240";

//NPT
const char* NTP_SERVER = "ch.pool.ntp.org";
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;

int lastH = -1;
int lastMin = -1;
int lastDate = -1;

void setup()
{
  //Serial.begin(115200);
  display.init();

  //Serial.println("\n\n SetupBegin \n");

  wifi();   //connect to wifi

  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);

  if (getNTPtime(10)) {  // wait up to 10sec to sync
  } else {
    Serial.println("Time not set");
    ESP.restart();
  }
  //showTime(timeinfo);
  lastNTPtime = time(&now);
  lastEntryTime = millis();
  
  display.eraseDisplay();
}

//light sleep

//############################################################
void loop() {
  Serial.println("\nStart");
  getNTPtime(10); //gets time
  
  int H = updateH(timeinfo);    //stores hours
  int Min = updateMin(timeinfo);    //stores minutes
  int Date = updateDate(timeinfo); 

  //REFRESH DATE
  if(Date != lastDate){
    refreshDate(timeinfo);
    lastDate = Date;
  }
  //REFRESH HOUR AND TIME
  else if(H != lastH){ //only changes display if hours has changed
    refreshH(timeinfo);
    lastH = H;
  }
  //REFRESH MIN
  else if(Min != lastMin){ //only changes display if minutes has changed
    refreshMin(timeinfo);
    lastMin = Min;
  }
  
  //WiFi.status() != WL_CONNECTED ? Serial.println("\nNoWLAN"): Serial.println("\nWLAN");
  //fulltime(timeinfo); //refresh whole time
  //test(timeinfo); //to test
  
  //showTime(timeinfo); //prints time in serial
  //showTime2(timeinfo);
  delay(30000);
}
//#######################################################################
void wifi(){
  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (++counter > 100) ESP.restart();
    Serial.print ( "." );
  }
  //Serial.println("\n\nWiFi connected\n\n");
}

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      //Serial.print(".");
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    if (timeinfo.tm_year <= (2016 - 1900)) return false;  // the NTP call was not successful
    //Serial.print("now ");  Serial.println(now);
    char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
    //Serial.println(time_output);
    //Serial.prin  tln();
  }
  return true;
}

int updateH(tm localTime){
  return localTime.tm_hour;
}
int updateMin(tm localTime){
  return localTime.tm_min;
}
int updateDate(tm localTime){
  return localTime.tm_mday;
}



//MAIN FUNCTIONS-------------------------------
void refreshH(tm localTime){

  //Serial.println("\nRefHOUR");
  
  int boxX = 0;
  int boxY = 10;
  int boxH = 30;
  int boxW = 120;
  
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 0);

  display.fillRect(boxX,boxY,boxW, boxH,GxEPD_WHITE);
  display.updateWindow(boxX,boxY,boxW, boxH, true);
  display.fillRect(boxX,boxY,boxW, boxH,GxEPD_BLACK);
  display.updateWindow(boxX,boxY,boxW, boxH, true);
  display.fillRect(boxX,boxY,boxW, boxH,GxEPD_WHITE);
  display.updateWindow(boxX,boxY,boxW, boxH, true);

  display.println();
  char s[25];
  sprintf(s, "%02d:%02d",localTime.tm_hour, localTime.tm_min);
  display.println(s);
  
  display.updateWindow(boxX,boxY,boxW, boxH, true);
}

void refreshMin(tm localTime){

  //Serial.println("\nRefMIN");
  
  int boxX = 60;
  int boxY = 10;
  int boxH = 30;
  int boxW = 50;
  
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 0);

  display.fillRect(boxX,boxY,boxW, boxH,GxEPD_WHITE);
  display.updateWindow(boxX,boxY,boxW, boxH, true);
  display.fillRect(boxX,boxY,boxW, boxH,GxEPD_WHITE);
  display.updateWindow(boxX,boxY,boxW, boxH, true);

  display.println();
  char s[25];
  sprintf(s, "%02d:%02d",localTime.tm_hour, localTime.tm_min);
  display.println(s);

  display.updateWindow(boxX,boxY,boxW, boxH, true);
  
}

void refreshDate(tm localTime){
  int boxX = 0;
  int boxY = 45;
  int boxH = 15;
  int boxW = 120;
  
  
  //Serial.print("RefDATE");
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(0, 60);
  int wd = localTime.tm_wday;

  if(wd==0){display.println(" SO");}
  else if(wd==1){display.println(" MO");}
  else if(wd==2){display.println(" DI");}
  else if(wd==3){display.println(" MI");}
  else if(wd==4){display.println(" DO");}
  else if(wd==5){display.println(" FR");}
  else if(wd==6){display.println(" SA");}
  
  char s[25];
  sprintf(s, " %02d/%02d",localTime.tm_mday, localTime.tm_mon + 1);
  display.println(s);

  display.update();
  //display.updateWindow(boxX,boxY,boxW, boxH, true);
}
void test(tm localTime){
  
}

void showTime(tm localTime) {
  Serial.print(localTime.tm_mday);
  Serial.print('/');
  Serial.print(localTime.tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime.tm_year - 100);
  Serial.print('-');
  Serial.print(localTime.tm_hour);
  Serial.print(':');
  Serial.print(localTime.tm_min);
  Serial.print(':');
  Serial.print(localTime.tm_sec);
  Serial.print(" Day of Week ");
  if (localTime.tm_wday == 0)   Serial.println(7);
  else Serial.println(localTime.tm_wday);
}



 // Shorter way of displaying the time
  void showTime2(tm localTime) {
  Serial.printf(
    "%04d-%02d-%02d %02d:%02d:%02d, day %d, %s time\n",
    localTime.tm_year + 1900,
    localTime.tm_mon + 1,
    localTime.tm_mday,
    localTime.tm_hour,
    localTime.tm_min,
    localTime.tm_sec,
    (localTime.tm_wday > 0 ? localTime.tm_wday : 7 ),
    (localTime.tm_isdst == 1 ? "summer" : "standard")
  );
  }
