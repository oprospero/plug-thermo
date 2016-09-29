#ifndef AT_CMD_H
#define AT_CMD_H

//typedef enum {
//    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
//    WL_IDLE_STATUS      = 0,
//    WL_NO_SSID_AVAIL    = 1,
//    WL_SCAN_COMPLETED   = 2,
//    WL_CONNECTED        = 3,
//    WL_CONNECT_FAILED   = 4,
//    WL_CONNECTION_LOST  = 5,
//    WL_DISCONNECTED     = 6
//} wl_status_t;

const char sATnr[] PROGMEM = "AT\n\r";
const char sATrn[] PROGMEM = "AT\r\n";
const char sATn[] PROGMEM = "AT\n";
const char sAT[] PROGMEM = "AT";

#endif
