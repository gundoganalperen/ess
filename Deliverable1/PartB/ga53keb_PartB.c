/******************************************************************************
 * Copyright (C) 2017 by Alperen Gündogan(ga53keb) - Technical University of Munich
 * 
 * Deliverable 1 of Embedded System and Security
 *
 *********************************************************************************/

/*
 * PartB of the deliverable
 * This code takes the string "I CAN MORSE" and the string is sent once after each press of Button1, not continuously.
 * If Button2 is pressed, the time between the last and second last press of Button1 in
 * milliseconds is sent once in Morse code via LED1.
 	* If Button1 has not been pressed yet, send a single 0 digit.
 	* If Button1 has only been pressed once, send the time between boot and the first
    * press of Button1.
    * The time has to be sent in decimal notation without leading zeroes or decimal point.
    * Round off to full milliseconds.
 * While a transmission of Morse code is ongoing, any button press may be ignored.
 * If both buttons are pressed simultaneously, the one pressed down first wins.
 * @author Alperen Gündogan
 * @date 14.11.2017
 *
*/
#include <xmc_gpio.h>	

#define LED1 P1_1
#define BUTTON1 P1_14
#define LED2 P1_0
#define BUTTON2 P1_15
#define elementDelay 100
#define TICKS_PER_SECOND 1000 // one tick means 1ms
#define ARRAY_SIZE 5
static volatile uint32_t ticks;

static volatile char sendTime[10]; // Store the time of pressing Button1 in the volatile char array.

volatile uint32_t temp_ticks;
      // Delay for one element in milliseconds (assuming 1 WPM)
                                  // Dot  = 1 element
                                  // Dash = 3 elements
                                  // Inter-symbol (dot/dash) spacing = 1 element
                                  // Inter-character spacing         = 3 elements
                                  // Inter-word spacing              = 7 elements 


// Define dots and dashes         // String index to Character
volatile char* dotsNdashes[] = {".-",      //  0 = A ('A' = ASCII code 65; 'a' = ASCII code 97)
                       "-...",    //  1 = B
                       "-.-.",    //  2 = C
                       "-..",     //  3 = D
                       ".",       //  4 = E
                       "..-.",    //  5 = F
                       "--.",     //  6 = G
                       "....",    //  7 = H
                       "..",      //  8 = I
                       ".---",    //  9 = J
                       "-.-",     // 10 = K
                       ".-..",    // 11 = L
                       "--",      // 12 = M
                       "-.",      // 13 = N
                       "---",     // 14 = O
                       ".--.",    // 15 = P
                       "--.-",    // 16 = Q
                       ".-.",     // 17 = R
                       "...",     // 18 = S
                       "-",       // 19 = T
                       "..-",     // 20 = U
                       "...-",    // 21 = V
                       ".--",     // 22 = W
                       "-..-",    // 23 = X
                       "-.--",    // 24 = Y
                       "--..",    // 25 = Z
                       "-----",   // 26 = 0 (ASCII code 48)
                       ".----",   // 27 = 1
                       "..---",   // 28 = 2
                       "...--",   // 29 = 3
                       "....-",   // 30 = 4
                       ".....",   // 31 = 5
                       "-....",   // 32 = 6
                       "--...",   // 33 = 7
                       "---..",   // 33 = 8
                       "----.",   // 34 = 9
                       "--..--",  // 36 = , (ASCII code 44)
                       ".-.-.-"   // 37 = . (ASCII code 46)
                      };
/* 
 * @brief Display the given string
 * This function takes volatile char string.
 * Then using dotsNdashes char array to determine the Morse symbol of the given string.
 * Then call the flashLed() function to set the high or low levels of the LED1.
 * @param txtString[] is the given string
*/
void displayThis (volatile char txtString[]) {
    for (int i = 0; txtString[i] != '\0'; i++)
      if (txtString[i] >= 'a' && txtString[i] <= 'z')
        txtString[i] = txtString[i] - 32;

    for (int i = 0; txtString[i] != '\0'; i++) {
      if (txtString[i] >= 'A' && txtString[i] <= 'Z')
	  {
        flashLED(dotsNdashes[txtString[i] - 'A']);
		if((txtString[i+1] == ' ') && (txtString[i+1] != '\0'))
		delay((elementDelay * 6)); // Inter-word delay = 7 x dot delays; we already have one after last symbol 
		else if((txtString[i+1] != ' ') && (txtString[i+1] != '\0'))
		delay((elementDelay * 2));   // Inter-character delay = 3 x dot delays; we already have one after last symbol
      }
      else if (txtString[i] >= '0' && txtString[i] <= '9')
      {
		flashLED(dotsNdashes[txtString[i] - '0' + 26]);
		if((txtString[i+1] != ' ') && (txtString[i+1] != '\0'))
		delay((elementDelay * 2)); // Inter-word delay = 7 x dot delays; we already have one after last symbol
	  } 
      else if (txtString[i] == ',')
        flashLED(dotsNdashes[36]);
      else if (txtString[i] == '.')
        flashLED(dotsNdashes[37]);
      else if (txtString[i] == ' ')
      {
	  }
	 //delay((elementDelay * 6)); // Inter-word delay = 7 x dot delays; we already have one after last symbol 
        
     //delay((elementDelay * 2));   // Inter-character delay = 3 x dot delays; we already have one after last symbol   
    }

  //delay((elementDelay * 6));       // Inter-word delay = 7 x dot delays; we already have one after last symbol
}
/* 
 * @brief determine the on/off status of the LED1 according to given letter.
 * This function takes the Morse code correspond of the given letter.
 * Then using the given Morse Code standards, set the on/off status.
 * This function uses the delay() function to determine the time of on/off status of LED1.
 * @param dNdStrinh[] is the morse code of the given letter.
*/
void flashLED (volatile char dNdString[]) {

  for (int i = 0; dNdString[i] != '\0'; i++) {
      XMC_GPIO_SetOutputHigh(LED1);         // Set the LED to its active state
    
    if (dNdString[i] == '.')            // It's a dot
      delay(elementDelay);        // Dot delay
    else if (dNdString[i] == '-')       // It's a dash
      delay((elementDelay * 3));  // Dash delay
   
      XMC_GPIO_SetOutputLow(LED1);          // Set the LED to its inactive state

    delay(elementDelay);          // Inter-symbol delay
  }
}
/*
 * @brief this function is called in every tick to count the time  
 * This function is used to determine the current time of the program in ms.
*/
void SysTick_Handler(void)
{
	ticks++;
}
/* 
 * @brief this is used to return the current number of ticks.
 * @return The current number of ticks
*/
static volatile uint32_t millis(void)
{
	return ticks;
}
/* 
 * @brief This function takes the desired delay time in milisecond
 * This is used to fix the on/off status time of LED1.
 * @param t takes the volatile unsigned long to fix the delay time of the program
*/
void delay (volatile unsigned long t)
{
 	uint32_t start, end;
	start = millis();
	end = start + t;
  	if (start < end)
	{ 
    	while ((millis() >= start) && (millis() < end)) 
		{ 
     		 // do nothing 
  		} 
  	}
	else 
	{ 
    	while ((millis() >= start) || (millis() < end)) 
		{
    	  	// do nothing
   		};
  	}
}
/*
unsigned int counter(unsigned int c)
{
	unsigned int temp = 0;
	
    while(c != 0)
	{
     c = (c/10);
	 temp++;	
	}
    if(c == 0);
	return 1;

return temp;
}
*/
int main(void)
{
	XMC_GPIO_SetMode(LED1, XMC_GPIO_MODE_OUTPUT_PUSH_PULL); //Set LED1 
	XMC_GPIO_SetOutputStrength(LED1, XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE);

	XMC_GPIO_SetMode(BUTTON1, XMC_GPIO_MODE_INPUT_TRISTATE); // Set Button1 (No internal pull device active )

	XMC_GPIO_SetMode(BUTTON2, XMC_GPIO_MODE_INPUT_TRISTATE); // Set Button2 (No internal pull device active )
	
	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND); //Use Systick_Config to count the time
	//sendTime = (char*)malloc(ARRAY_SIZE*sizeof(char));
	//volatile unsigned int i = 0;
	//volatile uint32_t button1Flags[100]; //Store the ticks value in the uint32_t array.
	temp_ticks = 0;
	int n;
	while(1)
	{	
		if(!XMC_GPIO_GetInput(BUTTON1)) //XMC_GPIO_GetInput(BUTTON1) = 1 when the button1 is not push
		{
			temp_ticks = millis()-temp_ticks;
			displayThis("I CAN MORSE");	
		}

		if((!XMC_GPIO_GetInput(BUTTON2)))
		{
			//sendTime = (char*)malloc(ARRAY_SIZE*(counter(temp_ticks))*sizeof(char));
			n =sprintf(sendTime,"%lu",temp_ticks);
			sendTime[n+1] = '\0';
			displayThis(sendTime);
		}		
	}

return 0;
}
