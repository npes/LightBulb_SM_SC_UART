/*
 * LightBulb_SM.cpp
 *
 * Created: 29-02-2016 13:24:20
 * Author : rust
 * Modified by: nisi 08-05-2017
 *
 *     ___ E_LIGHT_ON                                            ___
 *    /   \ ____________       E_LIGHT_OFF        _____________ /   \
 *   |     /            \ ---------------------> /             \     | E_LIGHT_OFF
 *   |     | S_LIGHT_ON |      E_LIGHT_ON        | S_LIGHT_OFF |     |
 *    \--> \____________/ <--------------------  \_____________/ <--/           
 *
 *   +----------------------------------------------------+
 *   | STATE\EVENT     | E_LIGHT_ON     | E_LIGHT_OFF     |
 *   |-----------------+----------------+-----------------|
 *   | STATE_LIGHT_ON  |    --------    | STATE_LIGHT_OFF |
 *   |-----------------+----------------+-----------------|
 *   | STATE_LIGHT_OFF | STATE_LIGHT_ON |    --------     |
 *   +----------------------------------------------------+       
 *
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include "STDIO_UART.h"
#include <util/delay.h>

/* The two states in the enum */
enum State 
{
	S_LIGHT_ON,
	S_LIGHT_OFF
}; 

/* The Two event assigned to the same enum */
enum Event
{
	NO_EVENT,	// Used for testing the buttons for button press
	E_BUTTON_ON,
	E_BUTTON_OFF
};



/* Function prototypes */
int get_button_press();
void handle_action(unsigned int state, unsigned int event);
unsigned int handle_SM(unsigned int state, unsigned int event);

int main(void)
{
	ioinit(); //initialize UART
	unsigned int state;
	unsigned int event;

	printf("Enum State has these values: \nS_LIGHT_ON = %d\nS_LIGHT_OFF = %d\n\n", S_LIGHT_ON, S_LIGHT_OFF);
	printf("Enum Event has these values: \nNO_EVENT = %d\nE_BUTTON_ON = %d\nE_BUTTON_OFF = %d\n\n", NO_EVENT, E_BUTTON_ON, E_BUTTON_OFF);
	
	/* Setup PC.4 - used to simulate light bulb on/off
	 * PC.4 high -> Light ON  - (LED ON)
	 * PC.4 low  -> Light OFF - (LED OFF)
	 */
	DDRC |= (1 << DDC5);    // Set PC.4 to output direction
	PORTC &= ~(1 << DDC5);  // Initially turn LED off

    state = S_LIGHT_OFF;	// Set state variable to initial state.

	
	/* Setup PD.4 and PD.5 as on/off buttons. 
	 * PD.4 (high->low transistion) Light ON
	 * PD.5 (high->low transistion) Light OFF
	 */ 
	PORTD |= (1 << DDD4) | (1 << DDD3);   // Enable pull-up resistor on PD.4 and PD.5.
	DDRD &= ~((1 << DDD4) | (1 << DDD3)); // Set pin 4 and 5 to input direction
	
	/* The buttons can now be setup in interrupt mode, or pulling mode. 
	 * The buttons in this example are implemented as pulling mode in a separate function
	 */
	
	/* Enter the loop testing for events and running the state machine */
    while (1) 
    {
		event = get_button_press();
		handle_action(state, event);
		state = handle_SM(state, event);
    }
}

/************************************************************************/
/* Do the action before entering the state                              */
/************************************************************************/
void handle_action(unsigned int state, unsigned int event)
{
	switch (state)
	{
		case S_LIGHT_ON:
		{
			switch (event)
			{
				case E_BUTTON_OFF:
					PORTC &= ~(1 << DDC5);	// Turn on the LED;
					//printf("Button OFF event detected, event is = %d \n", event);
					break;
					
				case E_BUTTON_ON: // Light bulb already on - do nothing
					//printf("Button OFF event detected, event is already = %d \n", event);
					break;
			}
		}
		break;
		
		case S_LIGHT_OFF:
		{
			switch (event)
			{
				case E_BUTTON_ON:
					PORTC |= (1 << DDC5); //led port high 
					//printf("Button ON event detected, event is = %d \n", event);
					break;
				
				case E_BUTTON_OFF: // Light bulb already off - do nothing
					//printf("Button ON event detected, event is is already = %d \n", event);
					break;
			}
		}
	}
}


/************************************************************************/
/* Handle the state machine according to the state and event            */
/* Returns the new state                                                */
/************************************************************************/
unsigned int handle_SM(unsigned int state, unsigned int event)
{
	unsigned int new_state = state;
	
	switch (state)
	{
		case S_LIGHT_ON:
		{
			switch (event)
			{
				case E_BUTTON_OFF:
					new_state = S_LIGHT_OFF; // set the state to E_LIGHT_ON
					printf("State is = %d, event is %d and light is OFF\n", state, event);
					break;
					
				case E_BUTTON_ON:
					/* Do nothing */
				break;
			}
		}
		break;
		
		case S_LIGHT_OFF:
		{
			switch (event)
			{
				case E_BUTTON_ON:
					new_state = S_LIGHT_ON;
					printf("State is = %d, event is %d and light is ON\n", state, event);
					break;
				
				case E_BUTTON_OFF:
					/* Do nothing */
				break;
			}			
		}
		break;
	}
	return new_state;
}

/************************************************************************/
/* Test PD.4 and PD.5 for a button press. Return the corresponding event */
/************************************************************************/
int get_button_press()
{
	int event = NO_EVENT;
	//printf("Event is = %d \n", event); //shows that NO_EVENT has the value 0
	while (event == NO_EVENT)
	{
		if ((PIND & (1 << DDB4)) == 0)
		{
			event = E_BUTTON_ON; 
			_delay_ms(100);
			printf("Button ON event detected \n");
		}
		else if ((PIND & (1 << DDB3)) == 0)
		{
			event = E_BUTTON_OFF;
			_delay_ms(100);
			printf("Button OFF event detected \n");
		}
		else
		{
			/* None of the buttons have been pressed - do nothing */ 			
		}
	}
	return event;
}

