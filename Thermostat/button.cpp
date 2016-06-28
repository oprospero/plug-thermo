// #include "button.h"
#include "thermostat.h"

void init_button()
{
	pinMode(BUTTON_PLUS_PIN, INPUT_PULLUP);
	pinMode(BUTTON_MINUS_PIN, INPUT_PULLUP);
	pinMode(BUTTON_SET_PIN, INPUT_PULLUP);
	pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
}

button_type detect_button()
{
	static button_type prev = NONE;
	static unsigned int time_button = 0;
	button_type b = NONE;

	bool button_in = digitalRead(BUTTON_PLUS_PIN);
  	// PT("Button Down: "); PTL( button_in);
	if (prev == NONE && button_in == LOW) //Press down
	{
		b = PLUS_DOWN;
		time_button = millis();
	}
	else if ((prev == PLUS_DOWN || prev == PLUS_HOLD) && button_in == LOW) //Hold
	{
		b = PLUS_HOLD;
	}
	else if ((prev == PLUS_DOWN || prev == PLUS_HOLD) && button_in == HIGH) //UP
	{
		b = PLUS_UP;
	}

	button_in = digitalRead(BUTTON_MINUS_PIN);
  	// PT("Button Up: "); PTL( button_in);
	if (prev == NONE && button_in == LOW) //Press down
	{
		b = MINUS_DOWN;
	}
	else if ((prev == MINUS_DOWN || prev == MINUS_HOLD) && button_in == LOW) //Hold
	{
		b = MINUS_HOLD;
	}
	else if ((prev == MINUS_DOWN || prev == MINUS_HOLD) && button_in == HIGH) //UP
	{
		b = MINUS_UP;
	}

	button_in = digitalRead(BUTTON_MODE_PIN);
	if (prev == NONE && button_in == LOW) //Press down
	{
		b = MODE_DOWN;
	}
	else if ((prev == MODE_DOWN || prev == MODE_HOLD) && button_in == LOW) //Hold
	{
		b = MODE_HOLD;
	}
	else if ((prev == MODE_DOWN || prev == MODE_HOLD) && button_in == HIGH) //UP
	{
		b = MODE_UP;
	}

	button_in = digitalRead(BUTTON_SET_PIN);
	if (prev == NONE && button_in == LOW) //Press down
	{
		b = SET_DOWN;
	}
	else if ((prev == SET_DOWN || prev == SET_HOLD) && button_in == LOW) //Hold
	{
		b = SET_HOLD;
	}
	else if ((prev == SET_DOWN || prev == SET_HOLD) && button_in == HIGH) //UP
	{
		b = SET_UP;
	}

	prev = b;
	return b;
}