//memory.cpp


#include "thermostat.h"

void init_mem()
{
	byte size = node_size();
	if (size > MAX_NODE) {
		node_RESET();
	}
}

//TODO will help reduce schedule which are close in time
int operator-(const schedule& left, const schedule& right)
{
	return 0;
}
bool operator>(const schedule& left, const schedule& right)
{
	PTS("day: ");
	PT(left.day); PTS("-");
	PTL(right.day);
	if (left.day > right.day)
		return true;
	else if (left.day < right.day)
		return false;
	else // (left.day == right.day)
	{
		PTS("hour: ");
		PT(left.hour); PTS("-");
		PTL(right.hour);
		if (left.hour > right.hour)
			return true;
		else if (left.hour < right.hour)
			return false;
		else
		{
			PTS("minute: ");
			PT(left.minute); PTS("-");
			PTL(right.minute);
			if (left.minute > right.minute)
				return true;
		}

	}
	return false;
}

bool operator>=(const schedule& left, const schedule& right)
{
	if (operator==(left, right))
		return true;
	else
		return operator>(left,right);
}

bool operator<(const schedule& left, const schedule& right)
{
	if (left.day < right.day)
		return true;
	else if (left.day > right.day)
		return false;
	else //if (left.day == right.day)
	{
		if (left.hour < right.hour)
			return true;
		else if (left.hour > right.hour)
			return false;
		else //if (left.hour == right.hour)
		{
			if (left.minute < right.minute)
				return true;
		}
	}
	// (left.day > right.day)
	return false;
}

bool operator<=(const schedule& left, const schedule& right)
{
	if (operator==(left, right))
		return true;
	else
		return operator<(left,right);
}

bool operator==(const schedule& left, const schedule& right)
{
	if (left.day != right.day ||
		left.hour != right.hour ||
		left.minute != right.minute)
		return false;
	else
		return true;
}
// schedule operator=(const schedule& right)
// {
// 	schedule left;
// 	left.day = right.day;
// 	left.hour = right.hour;
// 	left.minute = right.minute;
// 	left.temperature = right.temperature;
// 	return left;
// }


void node_setSize(byte size)
{
	EEPROM.write(MEM_SIZE_LOCATION,size);
}

/*
00:	1, 	8,	00,	50
01:	1, 	18, 00,	70
02:	2, 	8,	00,	50
03:	2, 	18, 00,	70
04:	3, 	8,	00,	50
05:	3, 	18, 00,	70
06:	4, 	8,	00,	50
07:	4, 	18, 00,	70
08:	5, 	8,	00,	50
09:	5, 	18,	00,	70
10:	6, 	8,	00,	50
11:	6, 	18, 00,	70
12:	7, 	8,	00,	50
13:	7, 	18, 00,	70
*/

void node_RESET()
{
	schedule s;
	s.day = 1;
	s.hour = 7;
	s.minute = 0;
	s.temperature = 50;
	node_write(0,s);

	s.day = 1;
	s.hour = 19;
	s.minute = 0;
	s.temperature = 60;
	node_write(1,s);

	for (int i = 2; i < 12; i++) {
		// schedule s;
		s.day = i / 2 + 1;
		s.minute = 0;
		if (i % 2  == 0) {
			s.hour = 8;
			s.temperature = 50;
		}
		else {
			s.hour = 18;
			s.temperature = 65;
		}
		node_write(i,s);
	}

	s.day = 1;
	s.hour = 8;
	s.minute = 0;
	s.temperature = 50;
	node_write(12,s);

	s.day = 1;
	s.hour = 19;
	s.minute = 0;
	s.temperature = 60;
	node_write(13,s);

	node_setSize(14);

}


int node_size()
{
	return EEPROM.read(MEM_SIZE_LOCATION);
}

void node_add(schedule s)
{
	PTLS("Adding node");
	const char size = node_size();
	char index = 0;
	if (size <= MAX_NODE)
	{
		while (index < size)
		{
			PTS("Index: "); PTL((int)index);
			schedule sofi = node_get(index);
			if (s > sofi)
			{
				PTLS("Greater");
				index++;
			}
			else if (s == sofi)
			{
				PTLS("Equal");
				int addr = MEM_FIRST_SCHEDULE_LOC;
		  		addr += sizeof(schedule) * index;
				EEPROM.write(addr+3,s.temperature);
				return;
			}
			else // (s < sofi)
			{
				PTLS("LESS");
				//shift scheuldes forward
				char shift_i = size;
				while (shift_i >= index)
				{
					PTS("Shift: "); PTL((int)shift_i);
					schedule temp = node_get(shift_i-1);
					node_write(shift_i,temp);
					shift_i--;
				}
				PTLS("Added");
				//add schedule
				node_write(index,s);
				break;
			}
		}
		// If we reach the end add it there
		if (index == size)
		{
			PTLS("Added to End");
			node_write(index,s);
		}
		node_setSize(size +1);
	}
	else if (size > MAX_NODE)
	{
		//IDEA: edit closest schedule with given s 
		// For now do nothing
		PTLS("Memory Maxed");
		return;
	}
}

schedule node_get(byte index)
{
	schedule result;
	unsigned int addr = MEM_FIRST_SCHEDULE_LOC;
	addr += sizeof(schedule) * index;
	EEPROM.get(addr,result);
	// result.day = EEPROM.read(addr);
	// result.hour = EEPROM.read(addr+1);
	// result.minute = EEPROM.read(addr+2);
	// result.temperature = EEPROM.read(addr+3);
	return result;
}

void node_write(byte index,schedule s)
{
  int addr = MEM_FIRST_SCHEDULE_LOC;
  addr += sizeof(schedule) * index;
  EEPROM.put(addr,s);
}

void node_delete(byte index)
{
	const byte size = EEPROM.read(MEM_SIZE_LOCATION);
	byte shift_i = index + 1;

	//shift other schedules back
	while (shift_i < size)
	{
		schedule temp = node_get(shift_i);
		node_write(shift_i - 1 ,temp);
		shift_i++;
	}
	node_setSize(size -1);
}

void node_edit(byte index, schedule s)
{
	int insert_index = 0;
	const byte size = node_size();
	schedule edit_s = node_get(index);
	if (s < edit_s)
	{
		insert_index = index;
		//Foward shift
		while(s < edit_s && insert_index > 0)
		{
			// Grab and check previous node
			edit_s = node_get(insert_index-1);
			if (s >= edit_s) // If new node is greater
				break;
			node_write(insert_index,edit_s);
			insert_index--;	
		} 
		node_write(insert_index, s);
	}
	else if (s > edit_s)
	{
		insert_index = index;
		//Back shift
		byte max_index = size - 1;
		while(s > edit_s && insert_index <= max_index)
		{
			edit_s = node_get(insert_index+1);
			if (s <= edit_s)
				break;
			node_write(insert_index,edit_s);
			insert_index++;	
		} 
		node_write(insert_index, s);
	}
	else // equal
	{
		//Change temp only  
		int addr = MEM_FIRST_SCHEDULE_LOC;
  		addr += sizeof(schedule) * index;
		EEPROM.write(addr+3,s.temperature);
		return;
	}

}