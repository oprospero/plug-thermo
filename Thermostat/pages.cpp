  #include "thermostat.h"


// Holds string values for weekdays
const String dayShortNames_P = "ErrSunMonTueWedThuFriSat";
long timeScale[3] = {SCALE_DAY, SCALE_HOUR, SCALE_MIN};
String menu [4] = {"Modify Schedule", "Add Schedule", "Delete Schedule", "Modify Time    "};

page_type page = HOME;
page_type pre_page = page_type::ERROR_PAGE;
boolean mod = false;
bool forceLCDupdate = true;

void switchPage(page_type _page) {
  PTS("Switching from "); PT(page);
  PTS(", to "); PTL(_page);
  page = _page;
  lcd.clear();
  //Without this the page will stay empty for a while
  // forceLCDupdate = true; 
}

String getTimeString(time_t t = now())
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

inline void page_home() {
  if (pre_page != HOME) {
    PTLS("Page Home");
    pre_page = HOME;
    lcd.noCursor();
    // updateNodeIndex(now());
  }
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

  if (button_input == PLUS_UP) {
    target_temperature++;
  }
  else if (button_input == MINUS_UP) {
    target_temperature--;
  }
  else if (button_input == SET_UP) {

  }
  else if (button_input == MODE_UP) {
  //        switchPage(MODIFY_TIME);
    switchPage(LIST_MODE);
  }
}

inline void page_listMode() {
  static char selected = 0;
  if(pre_page != LIST_MODE) {
    PTLS("Page List Modes");
    lcd.cursor();
    pre_page = LIST_MODE; 
    selected = 0;
  }
  lcd.print(menu[selected % 4]);
  lcd.setCursor(0,1);      
  lcd.print(menu[((selected+1) % 4)]);
  
  if(button_input == PLUS_UP) {
    selected += 1;
    selected =  selected % 4;
  } else if (button_input == MINUS_UP) {
    selected -= 1;
    if(selected == -1) {
      selected = 3;
    }
  } else if (button_input == MODE_UP) {
    switchPage(HOME);
  } else if (button_input == SET_UP) {
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
  
}

inline void page_pickSchedule() {
  static byte prev_index = 0;
  if(pre_page != PICK_SCHEDULE) {
    PTLS(" Page Pick Schedule");
    lcd.cursor();
    pre_page = PICK_SCHEDULE; 
    node_index = node_index % (node_size()-1);
    prev_index = node_index + 1;
  }
  
  if (prev_index != node_index) {
    prev_index == node_index;
    schedule s = node_get(node_index);
    String t = getTimeString(s);
    if (mod) lcd.print(F("Mod:"));
    else lcd.print(F("Del: "));
    lcd.print(t);
    lcd.setCursor(0,1);
    lcd.print("Targ Temp: " + (String)s.temperature);
  }
  
  if (button_input == PLUS_UP)
  {
    node_index += 1;
    if (node_index >= node_size())
      node_index = 0;
  }
  else if (button_input == MINUS_UP)
  {
    node_index -= 1;
    if(node_index < 0) {
      node_index = (node_size()-1);
    }
  }
  else if (button_input == SET_UP)
  {
    if(mod) {
      switchPage(MODIFY_SCHEDULE);
    } else {
      PTLS("Node deleted");
      node_delete(node_index);
      node_index = (node_size()-1);
      prev_index = node_index + 1;
    }
  }
  else if (button_input == MODE_UP)
  {
    switchPage(LIST_MODE);
  }
}

inline void page_modifySchedule(){
  static int value = 0;
  static schedule s = node_get(node_index);   
  if(pre_page != MODIFY_SCHEDULE) {
    PTLS("Page Modify Schedule");
    lcd.cursor();
    pre_page = MODIFY_SCHEDULE; 
    //Need to reintialize everytime we enter the page but not every refresh
    s = node_get(node_index);
    value = 0;
  }  
  String t = getTimeString(s);
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("Temp: " + (String) s.temperature);
  if (button_input == PLUS_UP) {
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
  } else if (button_input == MINUS_UP) {
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
      if (s.temperature <= 32 || s.temperature >= 99) 
      s.temperature = 32;
    }
  } else if (button_input == MODE_UP) {
    value += 1;
    value = value % 4;
  } else if (button_input == SET_UP) {
    node_edit(node_index,s);
    switchPage(PICK_SCHEDULE);
  }
}

inline void page_addSchedule() {
  static schedule s; //Schedule to be added
  static int value = 0;
  if(pre_page != ADD_SCHEDULE) {
    PTLS("Page Add Schedule");
    pre_page = ADD_SCHEDULE; 
    lcd.cursor();
  
    //Need to reintialize everytime we enter the page but not every refresh
    s.day = weekday();
    s.hour = hour();
    s.minute = minute();
    s.temperature = temperature_sensor;
    value = 0;
  }
  String t = getTimeString(s);
  lcd.print(F("Add: "));
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print(F("Temp: "));
  lcd.print(s.temperature);
  if (button_input == PLUS_UP) {
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
  else if (button_input == MINUS_UP) {
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
      if (s.temperature <= 32 || s.temperature >= 99) 
        s.temperature = 32;
    }
  } else if (button_input == MODE_UP)  {
    value += 1;
    value = value % 4;
  } else if (button_input == SET_UP) {
    node_add(s);
    switchPage(LIST_MODE);
  }
}

inline void page_modifyTime() {
  static int timeScaleIndex = 0;
  if (pre_page != MODIFY_TIME)
  {
    PTLS("Page Mod Time");
    pre_page = MODIFY_TIME;
    lcd.cursor();
  }
  lcd.setCursor(0,1);
  String str_time = getTimeString(); 
  lcd.print(str_time);

  if (timeScaleIndex == 0) lcd.setCursor(0,1);
  else if (timeScaleIndex == 1) lcd.setCursor(0,4);
  else if (timeScaleIndex == 2) lcd.setCursor(0,7);
    
  if (button_input == PLUS_UP)
  {
    long adjust = timeScale[timeScaleIndex];
    adjustTime(adjust);
    setSyncInterval(SCALE_HOUR);
  }
  else if (button_input == MINUS_UP)
  {
    long adjust = -timeScale[timeScaleIndex];
    adjustTime(adjust);
    setSyncInterval(SCALE_HOUR);
  }
  else if (button_input == SET_UP)
  {
    setSyncInterval( 3600 );
    switchPage(HOME);
  }
  else if (button_input == MODE_UP)
  {
    timeScaleIndex++;
    timeScaleIndex = timeScaleIndex % 3;
    PTS("Changed focus: "); PTL(timeScaleIndex);
  }
}

void updateLCDscreen() {

  static int last_update = 0;
  int diff = millis() - last_update;

  if (button_input != NONE
    || diff > 1000
    || forceLCDupdate) 
  {
    last_update = millis();
    if (forceLCDupdate) forceLCDupdate = false;

    //Moved out since we call this regardless of page.
    lcd.home(); //Resets cursor 
    // This switch cases are responsible for displaying different content
    // on the LCD
    switch (page) 
    {
      case HOME:
        page_home();
        break;
      
      case LIST_MODE:  
        page_listMode();
        break;
      
      case PICK_SCHEDULE:
        page_pickSchedule();
        break;

      case MODIFY_SCHEDULE:
        page_modifySchedule();
        break;

      case ADD_SCHEDULE:
        page_addSchedule();
        break;
      
      case MODIFY_TIME:
        page_modifyTime();
        break;
    }// switch (page)
  }
}
