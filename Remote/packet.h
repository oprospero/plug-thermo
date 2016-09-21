

/* 
[0] cmd
[1] value 1
[2] value 2
[3] value 3
[4] value 4
[5] packet number
*/

#define ADDR_BASE 0x01AABBCCDD
#define ADDR_REMOTE 0x02AABBCCDD
#define PACKET_SIZE 6
typedef unsigned char uchar;

// #define PKT_EMPTY 					0
// #define PKT_TEMPERATURE 				1
// #define PKT_SET_TEMPERATURE			2
// #define PKT_SET_TEMPERATURE_TIME 	3

enum cmd_type
{
	PKT_EMPTY=0,
	PKT_UPDATE_TEMPERATURE,
	PKT_UPDATE_HUMIDITY,
	PKT_SET_TARGET_TEMPERATURE,
	PKT_SET_CURRENT_TIME,
	PKT_SET_TEMPERATURE_TIME,
	PKT_GET_TIME=16,
	PKT_GET_NEXT_TEMPERATURE_TIME,
};



/* 
Setting Temp
[0] cmd
[1] value 1 - Temp
[2] value 2 - Day
[3] value 3 - Hour
[4] value 4 - Minute
[5] packet number
*/
