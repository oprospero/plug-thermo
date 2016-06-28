/*
Written by: Prosper Van
Description: This library is meant to encapsulate the need to call the RF24 api
directly for the Remote-Temperature-Sensing Plugin-Thermostat
*/
// #include "Comm.h"
#include "thermostat.h"


commBase::commBase() : com(RF_CE_PIN, RF_CSN_PIN)
{
}

void commBase::begin()
{
	com.begin();
	// Set the PA Level low to prevent power supply related issues since this is a
	// getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	com.setPALevel(RF24_PA_LOW);
  
	com.openWritingPipe(ADDR_BASE);
	com.openReadingPipe(1,ADDR_REMOTE);

	// Start the radio listening for data
	startListening();
}

cmd_type commBase::read()
{
	//Required - insures we can read the packets 
	startListening();

	cmd_type ret = PKT_EMPTY;
	byte packet[PACKET_SIZE]; //packet buffer
	if (com.available())
	{
		delay(5);
		com.read(packet, PACKET_SIZE);
	}
	else
		return ret;


	for (int i = 0; i < PACKET_SIZE; i++)
		PT(packet[i]);
	PTL();

	byte cmd = packet[0]; //first packet gives the type of packet
	if (cmd == PKT_UPDATE_TEMPERATURE)
	{
		PT("Updating Temp ");
		float * value = (float*)&packet[1];
		temperature_sensor = *value;
		ret = PKT_UPDATE_TEMPERATURE;
		PTL(*value);
	}

	// stopListening();
	return ret;
}

float commBase::getTemperature()
{
	return temperature_sensor;
}


void commBase::startListening()
{
	if (!isListening)
	{
		com.startListening();
		isListening = true;
	}
}

void commBase::stopListening()
{
	if (isListening)
	{
		com.stopListening();
		isListening = false;
	}
}