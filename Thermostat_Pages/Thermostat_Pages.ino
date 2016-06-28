#include <Mouse.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);

int node_index = 0;

void setup() {
  //List of stored schedules, set to store 3
  String menu [4] = {"Modify Schedule", "Add Schedule", "Delete Schedule", "Modify Time"};

}

  switch (page)
  {
    case HOME:
    {
      if (pre_page != HOME)
      {
        PTL("Page Home");
        pre_page = HOME;
        lcd.clear();
      }
      lcd.home();
      char str0[] = "Temp:";
      // char str1[] = "Time:";
      lcd.print(str0);
      lcd.print(temperature_sensor,1);
      lcd.print(" Tar:");
      lcd.print(target_temperature);
      lcd.setCursor(0,1);
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
        switchPage(LIST_MODE);
      }
      break;
    }// case HOME
    
    case LIST_MODE:
    {
      static int selected= 0;
      if(pre_page != LIST_MODE) {
        PTL(" Page List Modes");
        pre_page = LIST_MODE; 
        selected = 0;
      }
      lcd.print(menu[selected % 4]);
      lcd.setCursor(0,1);      
      lcd.print(menu[((selected+1) % 4)]);
      while(button.digitalRead() == LOW){
      }
      lcd.clear();
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
        if(selected % 4 == 0){
          switchPage(PICK_SCHEDULE);
        } else if (selected % 4 == 1) {
          switchPage(ADD_SCHEDULE);
        } else if (selected % 4 == 2) {
          switchPage(DELETE_SCHEDULE);
        } else {
          switchPage(MODIFY_TIME);
        }
      }
      
    }
    
    case PICK_SCHEDULE:
    {
      if(pre_page != PICK_SCHEDULE) {
        PTL(" Page Pick Schedule");
        pre_page = PICK_SCHEDULE; 
        lcd.clear();
      }
      node_index = node_index % (node_size()-1);
      schedule s = node_get(node_index);
      String t = s.hour + ": " + s.minute;
      lcd.print("Time: " + t);
      lcd.setCursor(0,1);
      lcd.print("Targ Temp:" + s.temperature);
      while(button.digitalRead() == LOW){
      }
      lcd.clear();
      if (button == PLUS_UP || button == PLUS_HOLD)
      {
        node_index += 1;
      }
      else if (button == MINUS_UP || button == MINUS_HOLD)
      {
        node_index -= 1;
        if(node_index < 0) {
          node_index = (node_size()-1);
        }
      }
      else if (button == SET_UP)
      {
        switchPage(MODIFY_SCHEDULE);
      }
      else if (button == MODE_UP)
      {
        switchPage(LIST_MODE);
      }
      
    }

    case MODIFY_SCHEDULE:
    {
      if(pre_page != MODIFY_SCHEDULE) {
        PTL(" Page Modify Schedule");
        pre_page = MODIFY_SCHEDULE; 
        lcd.clear();
      }
      static int value = 0;
      schedule s = node_get(node_index);     
      String t = s.hour + ": " + s.minute;
      lcd.print("Time: " + t);
      lcd.setCursor(0,1);
      lcd.print("Temp:" + s.temperature);      
      while(button.digitalRead() == LOW){
      }
      lcd.clear();
      if (button == PLUS_UP || button == PLUS_HOLD)
      {
        if (value % 3 == 0) {
          s.day++;
        } else if (value % 3 == 1) {
          s.hour++;
        } else {
          s.minute++;
        }
      }
      else if (button == MINUS_UP || button == MINUS_HOLD)
      {
        if (value % 3 == 0) {
          if(s.day > 0) {
            s.day--;
          }
        } else if (value % 3 == 1) {
          if(s.day > 0) {
            s.hour--;
          }
        } else {
          if(s.minute > 0) {
            s.minute--;
          }
        }
      }
      else if (button == SET_UP)
      {
        value += 1;
      }
      else if (button == MODE_UP)
      {
        switchPage(pre_page);
      }
    }
    
    case MODIFY_TIME:
    {
      static int timeScaleIndex = 0;
      if (pre_page != MODIFY_TIME)
      {
        PTL("Page Mod Time");
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
        PT("Changed focus: "); PTL(timeScaleIndex);
      }
    } //case MODIFY_TIME
    case MODIFY_TIME:
    {
      lcd.print(getTimeString());
      
      
  }// switch (page)
  delay(100);
}
