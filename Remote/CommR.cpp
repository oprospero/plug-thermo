#include "CommR.h"

commRemote::commRemote() : comm(RF_CE_PIN, RF_CS_PIN)
{
}

void commRemote::begin()
{	
	pkt_id = 0;
	Serial.begin(9600);
	Serial.println("Com Started");
	comm.begin();
	// Set the PA Level low to prevent power supply related issues since this is a
	// getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	comm.setPALevel(RF24_PA_LOW);
  
	comm.openWritingPipe(ADDR_REMOTE);
	comm.openReadingPipe(1,ADDR_BASE);

	// Start the radio listening for data
	// startListening();}
}

bool commRemote::sendTemperature(float temp)
{
	sendFloat(temp,PKT_UPDATE_TEMPERATURE);
}

bool commRemote::sendHumidity(float humy)
{
	sendFloat(humy,PKT_UPDATE_HUMIDITY);
}

bool commRemote::sendFloat(float value, cmd_type cmd)
{
	stopListening();
	byte packet[PACKET_SIZE];
	packet[0] = (byte) cmd;
	byte *pValue = (byte*)&value;
	for(int i = 0; i < 4; i++)
	{
		packet[i + 1] = pValue[i];
	}
	packet[PACKET_SIZE - 1] = pkt_id++;

	for (int i = 0; i < PACKET_SIZE; i++)
	{
		Serial.print(packet[i]);
		Serial.print(",");
	}
	Serial.println();

	comm.write(packet,PACKET_SIZE);
	startListening();
}

void commRemote::startListening()
{
	if (!isListening)
	{
		comm.startListening();
		isListening = true;
	}
}

void commRemote::stopListening()
{
	if (isListening)
	{
		comm.stopListening();
		isListening = false;
	}
}


// void commRemote::readSerial()
// {
	
// }