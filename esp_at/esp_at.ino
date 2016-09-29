
#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#define PTL   Serial.println
#define PT    Serial.print
#define WIFI_RETRY 10

char stringBuffer[50];
#define PMSTR(x) strcpy_P(stringBuffer,x)

//const char sATnr[]  PROGMEM = "AT\n\r";
//const char sATrn[]  PROGMEM = "AT\r\n";
//const char sATn[]   PROGMEM = "AT\n";
//const char sAT[]    PROGMEM = "AT";

const char _SSID[]  PROGMEM = "mines";
const char _PSWD[]  PROGMEM = "prospervan";

IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
int timeZone = -8; 

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

void setup() {
  Serial.begin(9600);
  Serial.println("\n\rESP started");
  WiFi.begin();

  
  setSyncProvider(getNtpTime);
  setSyncInterval(10);
}

void connectWifi()
{
  static bool upd_ok = false;
  wl_status_t stat = WiFi.status();
//  PT("Wifi status: "); PTL(stat);
  if (stat == WL_CONNECTED)
  {
    if (!upd_ok)
      Udp.begin(localPort);
  }
  else if (stat != WL_CONNECTED || stat == WL_DISCONNECTED)
  {
    char ssid[20];
    char pswd[20];
    strcpy_P(ssid,_SSID);
    strcpy_P(pswd,_PSWD);
    WiFi.begin(ssid, pswd);
    Udp.begin(localPort);
  }
}

bool str_cmp(const char* s1, const char* s2)
{
  int i = 0;
  while (s1[i] == s2[i])
  {
    if (s1[i] == NULL)
      break;
    i++;
  }
  if (s1[i] == NULL and s2[i] == NULL)
    return true;
  else
    return false;
}

void wifi_cmd()
{
  static char cmdBuffer[100];
  static byte bi = 0;
  static bool cmd_rdy = false;
  if (Serial.available() > 0)
  {
    char temp = Serial.read();
    if (temp == '\n')
    {
        cmd_rdy = true;
        cmdBuffer[bi++] = NULL;
    }
    if (temp == '\r');//Do nothing
    else
    {
      cmdBuffer[bi++] = temp;
    }
  }
  if (cmd_rdy)
  {
    cmd_rdy = false;
    Serial.println(cmdBuffer);
    if (str_cmp(cmdBuffer,("AT")))
    {
      Serial.println(F("OK"));
    }
    else if (str_cmp(cmdBuffer,("AT+TIME")))
    {
      time_t current = now();
      Serial.print("T:");
      byte *ptr = (byte*)&current;
      Serial.write(ptr,sizeof(current));
      Serial.println();
    }
    else //no match
    {
      Serial.println("ERROR");
    }
    bi = 0;
  }

}

void loop() {
   delay(10);
   wifi_cmd();
   delay(10);
   connectWifi();
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  static bool time_synced = false;
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
//  PTL("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
//      PTL("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      
      if (!time_synced && secsSince1900 > 60000)
      {
        time_synced = true;
        setSyncInterval(90000UL);
      }
      return secsSince1900 - 2208988800UL * SECS_PER_HOUR;
    }
  }
//  PTL("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
