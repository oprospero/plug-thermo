//memory.cpp


#include "thermostat.h"


//TODO will help reduce schedule which are close in time
int operator-(const schedule& left, const schedule& right)
{
	return 0;
}
bool operator>(const schedule& left, const schedule& right)
{
	if (left.day > right.day)
		return true;
	else if (left.day == right.day)
	{
		if (left.hour > right.hour)
			return true;
		else if (left.hour == right.hour)
		{
			if (left.minute > right.minute)
				return true;
			else
				return false;
		}
	}
	else // (left.day < right.day)
		return false;
}
bool operator<(const schedule& left, const schedule& right)
{
	if (left.day < right.day)
		return true;
	else if (left.day == right.day)
	{
		if (left.hour < right.hour)
			return true;
		else if (left.hour == right.hour)
		{
			if (left.minute < right.minute)
				return true;
			else
				return false;
		}
	}
	else // (left.day > right.day)
		return false;
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

int node_size()
{
	return EEPROM.read(MEM_SIZE_LOCATION);
}

void node_add(schedule s)
{
	const byte size = EEPROM.read(MEM_SIZE_LOCATION);
	char index = 0;
	if (size <= MAX_NODE)
	{
		while (index < size)
		{
			schedule sofi = node_get(index);
			if (s > sofi)
			{
				index++;
			}
			else if (s == sofi)
			{
				return;
			}
			else // (s < sofi)
			{
				//shift scheuldes forward
				byte shift_i = size;
				while (shift_i >= index)
				{
					schedule temp = node_get(shift_i-1);
					node_write(shift_i,temp);
					shift_i--;
				}
				//add schedule
				node_write(index,s);
				break;
			}
		}
		// If we reach the end add it there
		if (index == size)
		{
			node_write(index,s);
		}
		EEPROM.write(MEM_SIZE_LOCATION,size+1);
	}
	else if (size > MAX_NODE)
	{
		//IDEA: edit closest schedule with given s 
		// For now do nothing
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

}

void node_edit(byte index, schedule s)
{
	int insert_index = 0;
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
		node_write(insert_index, node_write)
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
		node_write(insert_index, node_write)
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