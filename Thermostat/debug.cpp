#include "Thermostat.h"

void check_serial_cmd()
{
	if (Serial.available())
	{
		delay(1);
		if (Serial.find("size"))
		{
			PTS("Schedule size: ");
			PTL(node_size());
		} else if (Serial.find("mem")) {
			PTLS("Reading");
			for (int i = 0; i < MAX_NODE; i++) {
				schedule s = node_get(i);
				PT(i); PTS(", ");
				PT(s.day); PTS(", ");
				PT(s.hour); PTS(", ");
				PT(s.minute); PTS(", ");
				PT(s.temperature); PTL();
			}

		} else if (Serial.find("RESET")) {
			PTLS("Resetting Memory");
			node_RESET();
		}
	}
}