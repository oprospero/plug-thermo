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

#define BUTTON_PLUS_PIN		5
#define BUTTON_MINUS_PIN	3
#define BUTTON_SET_PIN		2
#define BUTTON_MODE_PIN		4

#define RF_CE_PIN 9
#define RF_CSN_PIN 10

#define ESP_TX_PIN A0
#define ESP_RX_PIN A1
#define ESP_RST_PIN 7

extern float temperature_sensor;

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

void node_clear(byte);
void node_resort();
void node_delete(byte);
schedule node_get(byte);
void node_add(schedule);
void node_write(byte,schedule);
void node_edit(byte,schedule);


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
