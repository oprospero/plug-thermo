/*
Written by: Prosper Van
Description: This library is meant to encapsulate the need to call the RF24 api
directly for the Remote-Temperature-Sensing Plugin-Thermostat
*/

#ifndef	COMM_H
#define COMM_H

#define PT 			Serial.print
#define PTL 		Serial.println
#define PTS(x) 		Serial.print(F(x))
#define PTLS(x) 	Serial.println(F(x))

#include <SPI.h>
#include "RF24.h"
#include "Packet.h"
#include <SoftwareSerial.h>
#include <Time.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PLUS_PIN		5
#define BUTTON_MINUS_PIN	3
#define BUTTON_SET_PIN		2
#define BUTTON_MODE_PIN		4

#define RF_CE_PIN 9
#define RF_CSN_PIN 10

#define ESP_TX_PIN A0
#define ESP_RX_PIN A1
#define ESP_RST_PIN 7


#define RELAY_PIN 6 // Set to whichever pin is being used to control relay
#define ON HIGH
#define OFF LOW

#define SCALE_MIN   60
#define SCALE_HOUR  SCALE_MIN * 60
#define SCALE_DAY   86400
//const long SCALE_DAY = SCALE_HOUR * 24;

extern float temperature_sensor;
extern int target_temperature;
extern LiquidCrystal_I2C lcd;
extern char node_index;
extern char currentNodeIndex;

void updateLCDscreen();
void check_serial_cmd();

class commBase
{
public:
	commBase();

	// Initializes the nrf24 module along with the software serial
	// May want to modularize this. 
	void begin();

	// read()
	// This fuction will read incoming packets and will update necessary fields
	// As of now the only field is the temperture readings
	cmd_type read();
	// int read();
	float getTemperature();
	void startListening();
	void stopListening();

private:
	RF24 com;
	bool isListening;
	// byte packet[PACKET_SIZE];
};


#include <arduino.h>


#define HOLD_TIME 1000

enum button_type
{
	NONE,
	PLUS_DOWN,
	PLUS_HOLD,
	PLUS_UP,
	MINUS_DOWN,
	MINUS_HOLD,
	MINUS_UP,
	SET_DOWN,
	SET_HOLD,
	SET_UP,
	MODE_DOWN,
	MODE_HOLD,
	MODE_UP
};
extern button_type button_input;


void init_button();
button_type detect_button();


enum page_type
{
	HOME,
	LIST_MODE,
	MODIFY_LIST,
  	PICK_SCHEDULE,
	ADD_SCHEDULE,
	MODIFY_SCHEDULE,
	MODIFY_TIME,
	ERROR_PAGE
};

#define MEM_OFFSET 				100
#define MEM_SIZE_LOCATION 		MEM_OFFSET
#define MEM_FIRST_SCHEDULE_LOC 	MEM_SIZE_LOCATION + 1
#define MAX_NODE				28

struct schedule
{
	byte day;
	byte hour;
	byte minute;
	byte temperature;
};

int operator-(const schedule&, const schedule&);
bool operator>(const schedule&, const schedule&);
bool operator<(const schedule&, const schedule&);
bool operator>=(const schedule&, const schedule&);
bool operator<=(const schedule&, const schedule&);
bool operator==(const schedule&, const schedule&);
// schedule operator=(const schedule&);

void init_mem();
// Will reset schedules to default
void node_RESET();
// Adds a node, will kept structure sorted
void node_add(schedule);
// Will edit a node while keeping sorted structure
void node_edit(byte,schedule);
// Delete a node and shift the rest of the node
void node_delete(byte);
// The number of nodes in the list
int node_size(void);
// Grab node in the list of given index regarless of size
schedule node_get(byte);
// Writes a node to a particular index regardless of size
void node_write(byte,schedule);

void node_clear(byte);
void node_resort();

#define	ESP_NON			0
#define	ESP_FOUND		1
#define	ESP_CONNECTED 	2

#define ESP_NO_MODULE	0
#define ESP_FOUND_MOD	1
#define ESP_GOOD_TIME	2


// class ESP8266: public SoftwareSerial
// {
// public:
// 	ESP8266();
// 	void begin();
// 	bool check();
// 	bool connectAP(char ssid[], char pw[]);
// 	bool isConnected();
// 	bool isConnected(char ssid[]);
// 	bool CIPSTART(char ip[]);
// 	bool CIPSEND(String cmd);
// 	void CIPCLOSE();
// 	bool parseTime(time_t& tt);
// 	time_t getTime();

// private:
// 	int status;
// };

class ESP8266: public SoftwareSerial
{
public:
	ESP8266();
	void begin();
	bool check();
	bool parseTime(time_t& tt);
	time_t getTime();

private:
	int status;
};

// extern ESP8266 esp;
time_t espUpdateTime();

#endif
