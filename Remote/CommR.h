/*
Written by: Prosper Van
Description: This library is meant to encapsulate the need to call the RF24 api
directly for the Remote-Temperature-Sensing Plugin-Thermostat
*/

#ifndef COM_REMOTE_H_
#define COM_REMOTE_H_

// #include <Arduino.h>
// #include <SPI.h>
#include "RF24.h"
#include "packet.h"

#ifndef byte
typedef unsigned char byte;
#endif

#define RF_CE_PIN 9
#define RF_CS_PIN 10

class commRemote
{
public:
	commRemote();
	void begin();
// 	// readSerial();
	bool sendTemperature(float temp);
	bool sendHumidity(float humy);
	void startListening();
	void stopListening();


private:
	bool sendFloat(float value, cmd_type cmd);
	bool isListening;
	byte pkt_id;
	RF24 comm;
	// RF24 com(RF_CE_PIN, RF_CS_PIN);
	// RF24 com(9,10);
};

#endif