#include "CommR.h"
#include <DHT.h>


#define DHT_PIN A5

DHT dht(DHT_PIN, DHT11);
float temperature;
float humidity;
float heatIndex;
int t_lastSent = 0;
float avg_heatIndex = 0;
commRemote com;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialization started");
  com.begin();
  delay(1000);
  Serial.println("Initialization Finished");
}

void loop() {
  temperature = dht.readTemperature(true);
  humidity = dht.readHumidity();
  heatIndex = dht.computeHeatIndex(temperature, humidity);

  if (isnan(heatIndex))
  {
    Serial.print("No Temperature Readings");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.println(heatIndex);
    if (avg_heatIndex == 0)
    {
      avg_heatIndex = heatIndex;
    }
    avg_heatIndex = (avg_heatIndex + heatIndex) / 2;
//    com.sendTemperature(heatIndex);
  }
  
  if (isnan(humidity))
  {
    Serial.print("No Humidity Readings");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.println(humidity);
//    com.sendHumidity(humidity);
  }
  delay(3000);


  int t_now = millis();
  int t_diff = t_now - t_lastSent;
  // Send every 10 sec
  if (t_diff > 10000)
  {
    com.sendTemperature(heatIndex);
    com.sendHumidity(humidity);
  }
}
