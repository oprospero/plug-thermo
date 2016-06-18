//for esp with arduino firmware
#include "thermostat.h"


#define CONST_OK "OK"
#define DEVICE_SERIAL_BAUD 9600

ESP8266::ESP8266() : SoftwareSerial(ESP_TX_PIN, ESP_RX_PIN)
{
	status = ESP_NO_MODULE;
}

void ESP8266::begin()
{
  SoftwareSerial::begin(DEVICE_SERIAL_BAUD);
  SoftwareSerial::setTimeout(2000);
}

bool ESP8266::check()
{
	// PTLS("SENT OK");
	println("AT");
	if (find(CONST_OK))
	{
		// PTLS("FOUND OK");
		status = ESP_FOUND_MOD;
		return true;
	}
	// PTLS("AT FAILED");
	status = ESP_NO_MODULE;
	return false;
}


time_t ESP8266::getTime()
{	

	bool rel = false;
	time_t new_time;
	// PTL(status);
	if (status <= ESP_NO_MODULE)
	{
		// PTL("Esp not yet found");
		rel = check();
		return 0;
	}

	time_t result = 0;
  	byte *ptr = (byte*)&result;
	println("AT+TIME");
	if (find("T:"))
	{
		// PTL("Reading time");
		readBytes(ptr,4);
		PTL(result);
		return result;
	}
	else
	{
		// status = ESP_NO_MODULE;
	}
	return result;
}