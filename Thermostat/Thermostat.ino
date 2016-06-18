
//#include "Comm.h"
//#include "Display.h"
//#include "button.h"
#include <LiquidCrystal_I2C.h>
#include "thermostat.h"

#define SCALE_MIN   60
#define SCALE_HOUR  SCALE_MIN * 60
#define SCALE_DAY   86400
//const long SCALE_DAY = SCALE_HOUR * 24;

#define RELAY_PIN 6 // Set to whichever pin is being used to control relay
#define ON HIGH
#define OFF LOW

typedef unsigned char uchar;

commBase com;
page_type page = HOME;
page_type pre_page = page_type::ERROR_PAGE;
float temperature_sensor;
int target_temperature = 65;
float temp_offset = 1.5;
time_t cur_time;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ESP8266 esp;

long timeScale[3] = {SCALE_DAY, SCALE_HOUR, SCALE_MIN};

// Basic startup code
void setup() {
  Serial.begin(9600);
  PTLS("Initializing");

  com.begin();
  pinMode(RELAY_PIN, OUTPUT); 
  initialize_diplay();
  init_button();
  init_wifi();

  //Setting up callback for updating time
  setSyncProvider( (getExternalTime) espUpdateTime);
  setSyncInterval( 5 ); // Set small so we can sync asap
  
//  lcd.clear();
  lcd.print("Initialized");
  PTLS("Initialized");
}

//Forward declared function
String getTimeString(time_t t = now());

void loop() {

  com.read();
  if (temperature_sensor > target_temperature + temp_offset || temperature_sensor <= 10)
  {
//    last_state_on = false;
    digitalWrite(RELAY_PIN, OFF);
  }
  else if (temperature_sensor < target_temperature - temp_offset)
  {
//    last_state_on = true;
    digitalWrite(RELAY_PIN, ON);
  }
  
  
  // See if any buttons are pressed
  button_type button = detect_button();
//  PT("Button: "); PTL((int) button);
  
  // This switch cases are responsible for displaying different content
  // on the LCD
  switch (page)
  {
    case HOME:
    {
      if (pre_page != HOME)
      {
        PTLS("Page Home");
        pre_page = HOME;
        lcd.clear();
      }
      lcd.home(); //Resets cursor 
      const char str0[] = ("Temp:");
      // char str1[] = "Time:";
      lcd.print(str0);
      lcd.print(temperature_sensor,1);
      lcd.print(F(" Tar:"));
      lcd.print(target_temperature);
      lcd.setCursor(0,1); //Sets cursor to 2nd row 1st col
      // lcd.print(str1);
      
//      time_t ttt = espUpdateTime();
      String str_time = getTimeString(); 
//      PTL(str_time);
      lcd.print(str_time);
      
      if (button == PLUS_UP || button == PLUS_HOLD)
      {
        target_temperature++;
      }
      else if (button == MINUS_UP || button == MINUS_HOLD)
      {
        target_temperature--;
      }
      else if (button == SET_UP)
      {
        
      }
      else if (button == MODE_UP)
      {
        switchPage(MODIFY_TIME);
      }
      break;
    }// case HOME
    case MODIFY_TIME:
    {
      static int timeScaleIndex = 0;
      if (pre_page != MODIFY_TIME)
      {
        PTLS("Page Mod Time");
        pre_page = MODIFY_TIME;
        lcd.clear();
        lcd.cursor();
      }
      lcd.setCursor(0,1);
      String str_time = getTimeString(); 
      lcd.print(str_time);
      
      if (timeScaleIndex == 0) lcd.setCursor(0,1);
      else if (timeScaleIndex == 1) lcd.setCursor(0,4);
      else if (timeScaleIndex == 2) lcd.setCursor(0,7);
        
      if (button == PLUS_UP || button == PLUS_HOLD)
      {
        long adjust = timeScale[timeScaleIndex];
        adjustTime(adjust);
        setSyncInterval(SCALE_HOUR);
      }
      else if (button == MINUS_UP || button == MINUS_HOLD)
      {
        long adjust = -timeScale[timeScaleIndex];
        adjustTime(adjust);
        setSyncInterval(SCALE_HOUR);
      }
      else if (button == SET_UP)
      {
        setSyncInterval( 3600 );
        switchPage(HOME);
      }
      else if (button == MODE_UP)
      {
        timeScaleIndex++;
        timeScaleIndex = timeScaleIndex % 3;
        PTS("Changed focus: "); PTL(timeScaleIndex);
      }
    } //case MODIFY_TIME
  }// switch (page)
  delay(100);
}

void initialize_diplay()
{
 // initialize the lcd 
  lcd.init();
  lcd.print("Initializing");
  lcd.backlight();
  delay(1000);
  lcd.clear();
//  lcd.cursor();
}

String getTimeString(time_t t)
{
  // Holds string values for weekdays
  const String dayShortNames_P = "ErrSunMonTueWedThuFriSat";
  // String holder to return
  String dateStr;
  byte _day = weekday(t);
  byte sub1 = _day * 3;
  byte sub2 = sub1 + 3;
  dateStr = dayShortNames_P.substring(sub1, sub2);
  
  //Example "Sun "
  dateStr.concat(" ");

  int _hour = hour(t);
  if(_hour < 10) {
    dateStr.concat(" ");
  }
  dateStr.concat(_hour); // EX: "Sun 10"
  dateStr.concat(":");  // EX: "Sun 10:"
  
  int _minute = minute(t);
  if(_minute < 10) {
    dateStr.concat("0");
  }
  dateStr.concat(_minute); // EX: "Sun 10:1"
  dateStr.concat(":"); // EX: "Sun 10:1:"
  
  int _second = second(t);
  if(_second < 10) {
    dateStr.concat("0");
  }
  dateStr.concat(_second); // EX: "Sun 10:1:20"
  
  return dateStr;
}

void init_wifi()
{
  esp.begin();
  esp.check();
}

time_t espUpdateTime()
{
//  PTL("Updating time");
  static bool time_synced = false;
  time_t t = esp.getTime();

  //After we have synced once, we will set it to
  // sync ever hour 
  // Add extra check to insure time is valid
  if (!time_synced && t > 60000) 
  {
    time_synced = true;
    setSyncInterval(3600);
  }
  return t;
}

void switchPage(page_type _page) {
  page = _page;
  lcd.clear();
}


