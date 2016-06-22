//Work on toString function for printing schedules values


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
int node_index = 0;
boolean mod = false;
time_t cur_time;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ESP8266 esp;

// Holds string values for weekdays
const String dayShortNames_P = "ErrSunMonTueWedThuFriSat";
long timeScale[3] = {SCALE_DAY, SCALE_HOUR, SCALE_MIN};
String menu [4] = {"Modify Schedule", "Add Schedule", "Delete Schedule", "Modify Time"};

// Basic startup code
void setup() {
  Serial.begin(9600);
  PTLS("Initializing");

  com.begin();
  pinMode(RELAY_PIN, OUTPUT); 
  initialize_diplay();
  init_button();
  init_wifi();

  setSyncInterval( 5 ); // Set small so we can sync asap
  //Setting up callback for updating time
  setSyncProvider( (getExternalTime) espUpdateTime);
  
//  lcd.clear();
  lcd.print("Initialized");
  PTLS("Initialized");
}

//Forward declared function
String getTimeString(time_t t = now());

void loop() {
  check_serial_cmd();
  com.read();
  if (temperature_sensor > target_temperature + temp_offset || temperature_sensor <= 10)  {
//    last_state_on = false;
    digitalWrite(RELAY_PIN, OFF);
  } else if (temperature_sensor < target_temperature - temp_offset)  {
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
    case HOME: {
      if (pre_page != HOME) {
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
      
      if (button == PLUS_UP || button == PLUS_HOLD) {
        target_temperature++;
      }
      else if (button == MINUS_UP || button == MINUS_HOLD) {
        target_temperature--;
      }
      else if (button == SET_UP) {
        
      }
      else if (button == MODE_UP) {
//        switchPage(MODIFY_TIME);
        switchPage(LIST_MODE);
      }
      break;
    }// case HOME
    
    case LIST_MODE:  
    {
      static int selected= 0;
      if(pre_page != LIST_MODE) {
        PTLS("Page List Modes");
        pre_page = LIST_MODE; 
        selected = 0;
      }
      lcd.home();
      lcd.print(menu[selected % 4]);
      lcd.setCursor(0,1);      
      lcd.print(menu[((selected+1) % 4)]);
      
      if(button == PLUS_UP || button == PLUS_HOLD) {
        selected += 1;
        selected =  selected % 4;
      } else if (button == MINUS_UP || button == MINUS_HOLD) {
        selected -= 1;
        if(selected == -1) {
          selected = 3;
        }
      } else if (button == MODE_UP) {
        switchPage(HOME);
      } else if (button == SET_UP) {
        if(selected % 4 == 0){ //Modify
          mod = true;
          switchPage(PICK_SCHEDULE);
        } else if (selected % 4 == 1) { //Add
          switchPage(ADD_SCHEDULE);
        } else if (selected % 4 == 2) { //Delete
          mod = false;
          switchPage(PICK_SCHEDULE);
        } else {
          switchPage(MODIFY_TIME);
        }
      }
      
      break;
    } //case LIST_MODE:
    
    case PICK_SCHEDULE:
    {
      if(pre_page != PICK_SCHEDULE) {
        PTLS(" Page Pick Schedule");
        pre_page = PICK_SCHEDULE; 
        lcd.clear();
      }
      //node_index = node_index % (node_size()-1);
      node_index = node_index % (3-1);
      schedule s = node_get(node_index);
      String t = getTimeString(s);
      lcd.clear();
      lcd.print(t);
      lcd.setCursor(0,1);
      lcd.print("Targ Temp:" + (String)s.temperature);
      if (button == PLUS_UP || button == PLUS_HOLD)
      {
        node_index += 1;
      }
      else if (button == MINUS_UP || button == MINUS_HOLD)
      {
        node_index -= 1;
        if(node_index < 0) {
          //node_index = (node_size()-1);
          node_index =(3-1);
        }
      }
      else if (button == SET_UP)
      {
        if(mod) {
          switchPage(MODIFY_SCHEDULE);
        } else {
          node_delete(node_index);
        }
      }
      else if (button == MODE_UP)
      {
        switchPage(LIST_MODE);
      }
      
      break;
    } 

    case MODIFY_SCHEDULE:
    {
      static int value = 0;
      static schedule s = node_get(node_index);   
      if(pre_page != MODIFY_SCHEDULE) {
        PTLS("Page Modify Schedule");
        pre_page = MODIFY_SCHEDULE; 
        lcd.clear();

        //Need to reintialize everytime we enter the page but not every refresh
        s.day = weekday();
        s.hour = hour();
        s.minute = minute();
        s.temperature = temperature_sensor;
        value = 0;
      }  
      String t = getTimeString(s);
      lcd.clear();
      lcd.print(t);
      lcd.setCursor(0,1);
      lcd.print("Temp:" + s.temperature);      
      if (button == PLUS_UP || button == PLUS_HOLD) {
        if (value == 0) {
          s.day++;
          if (s.day >= 8) s.day = 1;
        } else if (value == 1) {
          s.hour++;
          if (s.hour >= 24) s.hour = 0;
        } else if (value == 2) {
          s.minute++;
          if (s.minute >= 60) s.minute = 0;
        } else if (value == 3) {
          s.temperature++;
          if (s.temperature >= 99) 
            s.temperature = 99;
        }
      } else if (button == MINUS_UP || button == MINUS_HOLD) {
        if (value == 0) {
          s.day--;
          if (s.day <= 0) s.day = 7;
        } else if (value == 1) {
          s.hour--;
          if (s.hour >= 24) s.hour = 23;
        } else if (value == 2) {
          s.minute--;
          if (s.minute >= 60) s.minute = 59;
        } else if (value == 3) {
          s.temperature--;
          if (s.temperature <= 32) 
          s.temperature = 32;
        }
      } else if (button == SET_UP) {
        value += 1;
        value = value % 4;
      } else if (button == MODE_UP) {
        switchPage(PICK_SCHEDULE);
      }
      break;
    } 

    case ADD_SCHEDULE:
    {
      static schedule s; //Schedule to be added
      static int value = 0;
      if(pre_page != ADD_SCHEDULE) {
        PTLS("Page Add Schedule");
        pre_page = ADD_SCHEDULE; 
        lcd.clear();
        
        //Need to reintialize everytime we enter the page but not every refresh
        s.day = weekday();
        s.hour = hour();
        s.minute = minute();
        s.temperature = temperature_sensor;
        value = 0;
      }
      String t = getTimeString(s);
      lcd.home();
      lcd.print(t);
      lcd.setCursor(0,1);
      lcd.print(F("Temp: "));
      lcd.print(s.temperature);
      if (button == PLUS_UP || button == PLUS_HOLD) {
        if (value == 0) {
          s.day++;
          if (s.day >= 8) s.day = 1;
        } else if (value == 1) {
          s.hour++;
          if (s.hour >= 24) s.hour = 0;
        } else if (value == 2) {
          s.minute++;
          if (s.minute >= 60) s.minute = 0;
        } else if (value == 3) {
          s.temperature++;
          if (s.temperature >= 99) 
            s.temperature = 99;
        }
      }
      else if (button == MINUS_UP || button == MINUS_HOLD) {
        if (value == 0) {
          s.day--;
          if (s.day <= 0) s.day = 7;
        } else if (value == 1) {
          s.hour--;
          if (s.hour >= 24) s.hour = 23;
        } else if (value == 2) {
          s.minute--;
          if (s.minute >= 60) s.minute = 59;
        } else if (value == 3) {
          s.temperature--;
          if (s.temperature <= 32) 
            s.temperature = 32;
        }
      } else if (button == MODE_UP)  {
        value += 1;
        value = value % 4;
      } else if (button == SET_UP) {
        node_add(s);
        switchPage(LIST_MODE);
      }
      break;
    }
    
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
      break;
    } //case MODIFY_TIME
  }// switch (page)
  delay(100);
}

void initialize_diplay()
{
 // initialize the lcd 
  lcd.init();
  lcd.print(F("Initializing"));
  lcd.backlight();
  delay(1000);
  lcd.clear();
//  lcd.cursor();
}

String getTimeString(time_t t)
{
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

String getTimeString(schedule &s)
{
  // String holder to return
  String dateStr;
  byte _day = s.day;
  byte sub1 = _day * 3;
  byte sub2 = sub1 + 3;
  dateStr = dayShortNames_P.substring(sub1, sub2);
  
  //Example "Sun "
  dateStr.concat(" ");

  int _hour = s.hour;
  if(_hour < 10) {
    dateStr.concat(" ");
  }
  dateStr.concat(_hour); // EX: "Sun 10"
  dateStr.concat(":");  // EX: "Sun 10:"
  
  int _minute = s.minute;
  if(_minute < 10) {
    dateStr.concat("0");
  }
  dateStr.concat(_minute); // EX: "Sun 10:1"
    
  return dateStr;
}
void init_wifi()
{
  esp.begin();
  esp.check();
}

time_t espUpdateTime()
{
  PTLS("Updating time");
  static bool time_synced = false;
  time_t t = esp.getTime();
  PTL(t);
  //After we have synced once, we will set it to
  // sync ever hour 
  // Add extra check to insure time is valid
  if (!time_synced && t > 60000) 
  {
    PTLS("Sync good");
    time_synced = true;
    setSyncInterval(3600);
  }
  return t;
}

void switchPage(page_type _page) {
  PTS("Switching from "); PT(page);
  PTS(", to "); PTL(_page);
  page = _page;
  lcd.clear();
}


