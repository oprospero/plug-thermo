//Work on toString function for printing schedules values


//#include "Comm.h"
//#include "Display.h"
//#include "button.h"
#include "thermostat.h"



typedef unsigned char uchar;

commBase com;
button_type button_input;
float temperature_sensor;
int target_temperature = 65;
float temp_offset = 1.5;
char node_index = 0;
char currentNodeIndex = 0;
time_t cur_time;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ESP8266 esp;


// Basic startup code
void setup() {
  Serial.begin(9600);
  PTLS("Initializing");
  
  com.begin();
  pinMode(RELAY_PIN, OUTPUT); 
  initialize_diplay();
  init_button();
  init_wifi();
  init_mem();
  setSyncInterval( 5 ); // Set small so we can sync asap
  //Setting up callback for updating time
  setSyncProvider( (getExternalTime) espUpdateTime);
  
//  lcd.clear();
  lcd.print("Initialized");
  PTLS("Initialized");
}


void loop() {
  delay(50);
  check_serial_cmd();
  
  //Update temperature reading
  com.read(); //Maybe split into more functions?

  //Update schedule to new target temperature
  updateTargetTemperature();

  if (temperature_sensor > target_temperature + temp_offset || temperature_sensor <= 10)  {
//    last_state_on = false;
    digitalWrite(RELAY_PIN, OFF);
  } else if (temperature_sensor < target_temperature - temp_offset)  {
//    last_state_on = true;
    digitalWrite(RELAY_PIN, ON);
  }
  
  // See if any buttons are pressed
  button_input = detect_button();
//  PT("Button: "); PTL((int) button);

  updateLCDscreen();
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

inline void init_wifi()
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
    updateNodeIndex(t);
  }
  return t;
}


inline void updateTargetTemperature() {
  time_t t = now();
  schedule s_now = convertTimeFormat(t);
  char nextNodeIndex = currentNodeIndex + 1;
  if (nextNodeIndex >= node_size()) {
    nextNodeIndex = 0;
  }
  schedule nextNode = node_get(nextNodeIndex);
  if (s_now >= nextNode){
    currentNodeIndex = nextNodeIndex;
    target_temperature = nextNode.temperature;
    PTS("Current node: ");
    PTL(currentNodeIndex, DEC);
    PTS("Target Temp: ");
    PTL(target_temperature, DEC);
  }
}

void updateNodeIndex(time_t t) {
  schedule s_now = convertTimeFormat(t);
  schedule nextTarg;
  char thisNodeIndex = -1;
  do {
    thisNodeIndex++;
    nextTarg = node_get(thisNodeIndex);
  } while (s_now >= nextTarg);
  PTS("Current node: ");
  PTL(thisNodeIndex, DEC);
  currentNodeIndex = thisNodeIndex;
  
}

schedule convertTimeFormat(time_t &t) {
  schedule s;
  s.day = weekday(t);
  s.hour = hour(t);
  s.minute = minute(t);
  return s;
}

