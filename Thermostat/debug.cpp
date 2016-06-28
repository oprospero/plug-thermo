#include "Thermostat.h"

void check_serial_cmd()
{
	while (Serial.available())
	{
		delay(1);
		static String command;
		char input = Serial.read();

		if (input == '\r')
		{

		} 
		else if (input == '\n')
		{
			if (command.equals(("size")))
			{
				PTS("Schedule size: ");
				PTL(node_size());
			} else if (command.equals(("mem"))) {
				PTLS("Reading");
				for (int i = 0; i < MAX_NODE; i++) {
					schedule s = node_get(i);
					PT(i); PTS(", ");
					PT(s.day); PTS(", ");
					PT(s.hour); PTS(", ");
					PT(s.minute); PTS(", ");
					PT(s.temperature); PTL();
				}
			} else if (command.equals(("RESET"))) {
				PTLS("Resetting Memory");
				node_RESET();
			}
			command = "";
		}
		else
		{
			command.concat(input);
		}

	}
}