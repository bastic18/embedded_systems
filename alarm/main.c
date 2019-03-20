/*
 * alarm.c
 *
 * Created: 10/28/2018 11:48:56 AM
 * Author : Alex Grant
 */ 


#define F_CPU 8000000UL
#include "lcd.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

void init_ports();
void init_timers();
void displayTime();
                        //  0     1    2    3   4    5    6    7    8     9
unsigned char DISPLAY[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};   // ASCII characters stored in the DISPLAY array

//unsigned char startUP = 0;

// seconds
unsigned char seconds_counter = 0;
unsigned char seconds_zero_val = 0;
unsigned char seconds_one_val = 0;
unsigned char seconds_zero = 0;
unsigned char seconds_one = 0;

// minutes
unsigned char minutes_counter = 0;
unsigned char minutes_zero_val = 0;
unsigned char minutes_one_val = 0;
unsigned char minutes_zero = 0;
unsigned char minutes_one = 0;

// hours
unsigned char hours_counter = 12;
unsigned char hours_zero_val = 0;
unsigned char hours_one_val = 0;
unsigned char hours_zero = 0;
unsigned char hours_one = 0;

//set Time
_Bool setTime = true;

// debounce
_Bool debounce_button0 = false;
_Bool debounce_button1 = false;
unsigned char button0_pressed = 0;
unsigned char button1_pressed = 0;

// Displays
// Main Displays
unsigned char main_menu_displays = 0;
unsigned char on_screen_clock_display = 0;
unsigned char time_settings_display = 1;
unsigned char alarm1_settings_display = 2;
unsigned char alarm2_settings_display = 3;
///////////////////////////////////////////
unsigned char display = 0;
///////////////////////////////////////////
//unsigned char clock_settings_display = 4;
// Alarm 1 variables
unsigned char alarm_1_hour_counter = 0;
unsigned char alarm_1_hour_one_val = 0x30;
unsigned char alarm_1_hour_zero_val = 0x30;
unsigned char alarm_1_minute_counter = 0;
unsigned char alarm_1_minute_one_val = 0x30;
unsigned char alarm_1_minute_zero_val = 0x30;
// Alarm 2 variables
unsigned char alarm_2_hour_counter = 0;
unsigned char alarm_2_hour_one_val = 0x30;
unsigned char alarm_2_hour_zero_val = 0x30;
unsigned char alarm_2_minute_counter = 0;
unsigned char alarm_2_minute_one_val = 0x30;
unsigned char alarm_2_minute_zero_val = 0x30;

unsigned char alarm_1_state = 0;
unsigned char alarm_2_state = 0;
unsigned char off = 0;
unsigned char on = 1;

unsigned char status_of_alarm_1 = 0;
unsigned char status_of_alarm_2 = 0;
unsigned char deactive = 0;
unsigned char active = 1;

// Checked
////////////////////////////////////////
unsigned char menu_selected = 0;
unsigned char main_menu_selected = 0;
//unsigned char on_screen_clock_menu = 1;
unsigned char setTime_menu = 2;
unsigned char alarm_1_menu = 3;
unsigned char alarm_2_menu = 4;
/////////////////////////////////////////////
////////////////////////////////////////////
// Minor displays
/*
// menus
unsigned char on_screen_clock_display_menu = 1;
unsigned char setTime_menu 
*/

// Time settings menu
unsigned char time_settings_menu = 0;
unsigned char setTime_hour_highlighted = 0;
unsigned char setTime_minute_highlighted = 1;
unsigned char setTime_second_highlighted = 2;
unsigned char setTime_set_time_highlighted = 3;

// Alarm 1 settings menu
unsigned char alarm_1_settings_menu = 0;
unsigned char alarm_1_hour_highlighted = 0;
unsigned char alarm_1_minute_highlighted = 1;
unsigned char alarm_1_set_highlighted = 2;
unsigned char alarm_1_off_highlighted = 3;

// Alarm 2 settings menu
unsigned char alarm_2_settings_menu = 0;
unsigned char alarm_2_hour_highlighted = 0;
unsigned char alarm_2_minute_highlighted = 1;
unsigned char alarm_2_set_highlighted = 2;
unsigned char alarm_2_off_highlighted = 3;

void display_alarm_1_time();
void display_alarm_2_time();
void beeping_alarm();

unsigned char previous_alarm_1_hour_counter_counter = 0;
unsigned char current_alarm_1_hour_counter = 0;
unsigned char previous_alarm_1_minute_counter = 0;
unsigned char current_alarm_1_minute_counter = 0;

unsigned char previous_alarm_2_hour_counter_counter = 0;
unsigned char current_alarm_2_hour_counter = 0;
unsigned char previous_alarm_2_minute_counter = 0;
unsigned char current_alarm_2_minute_counter = 0;

unsigned char seconds_alarm_1_on = 0;
unsigned char seconds_alarm_2_on = 0;

unsigned char alarm_1_beeping_time = 0;
unsigned char alarm_2_beeping_time = 0;
unsigned char sixty_seconds_not_reached = 0;
unsigned char sixty_seconds_reached = 1;
//unsigned char alarm_1_change = 0;
//unsigned char alarm_2_change = 0;
//unsigned char active = 0;

void create_symbol()
{
	lcdCommand (0x40); //Load CGRAM first address
	lcdData(0x04); //Edit row 1 data to CGRAM
	lcdData(0x0E); //Edit row 2 data
	lcdData(0x0E); //Edit row 3 data
	lcdData(0x0E); //Edit row 4 data
	lcdData(0x1F); //Edit row 5 data
	lcdData(0x00); //Edit row 6 data
	lcdData(0x04); //Edit row 7 data
	lcdData(0x00); //Edit row 8 data
}
int main(void)
{
	init_ports();
	init_timers();
	lcd_init();
	create_symbol();
	
	sei();                 // Enables global interrupts
	
    while(1)
    {
		menu_selected = main_menu_selected;
		while((PIND & (1<<PIND0))==1 && menu_selected==main_menu_selected){
			beeping_alarm();
			// Splitting of values into individual digits
			seconds_zero_val = seconds_counter%10;
			seconds_one_val = seconds_counter/10;
			
			minutes_zero_val = minutes_counter%10;
			minutes_one_val = minutes_counter/10;
			
			hours_zero_val = hours_counter%10;
			hours_one_val = hours_counter/10;
			
			// Conversion of values to ASCII code
			seconds_zero = DISPLAY[seconds_zero_val];
			seconds_one =  DISPLAY[seconds_one_val];
			
			minutes_zero = DISPLAY[minutes_zero_val];
			minutes_one = DISPLAY[minutes_one_val];
			
			hours_zero = DISPLAY[hours_zero_val];
			hours_one = DISPLAY[hours_one_val];
			
			lcd_gotoxy(1,1);
			lcdData(hours_one);
			lcdData(hours_zero);
			lcd_print(":");
			lcdData(minutes_one);
			lcdData(minutes_zero);
			lcd_print(":");
			lcdData(seconds_one);
			lcdData(seconds_zero);
			lcd_gotoxy(1,2);
			lcd_print("HR MN SEC");
			//PORTB |= ~(1<<PORTB0);
		}
		if(alarm_1_state==off && alarm_2_state==off){
			//if((PIND & (1<<PIND0))==0){
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Click to update");
			lcd_gotoxy(1,2);
			lcd_print("Time");
			//}
			_delay_ms(500);
			//while((PIND & (1<<PIND0))==1 && (PIND & (1<<PIND1))==1);
			while(PIND==0b00000011);
			//while((PIND & (1<<PIND0))==1);
			//_delay_ms(30);
			/////////////////////////////////////////////////////////////////////
			// Set Time Menu Option
			/////////////////////////////////////////////////////////////////////
			if((PIND & (1<<PIND1))==0){
				menu_selected = setTime_menu;
				time_settings_menu = setTime_hour_highlighted;
				TIMSK &= 0xFF^(1<<OCIE1A);
				_delay_ms(300);
				clear_lcd();
				//_delay_ms(300);
				while(menu_selected==setTime_menu){
					while(time_settings_menu==setTime_hour_highlighted){
						displayTime();
						lcd_gotoxy(1,2);
						lcd_print("^^");
						while(PIND==0b00000011);
						if((PIND & (1<<PIND1))==0){
							if(hours_counter==23){
								hours_counter = 0;
							}
							else{
								hours_counter++;
							}
							
						}
						else
						{
							time_settings_menu = setTime_minute_highlighted;
						}
						_delay_ms(300);
					}
					clear_lcd();
					while(time_settings_menu==setTime_minute_highlighted){
						displayTime();
						lcd_gotoxy(4,2);
						lcd_print("^^");
						while(PIND==0b00000011);
						if((PIND & (1<<PIND1))==0){
							if(minutes_counter==59){
								minutes_counter = 0;
							}
							else{
								minutes_counter++;
							}
							
						}
						else
						{
							time_settings_menu = setTime_second_highlighted;
						}
						_delay_ms(300);
					}
					clear_lcd();
					while(time_settings_menu==setTime_second_highlighted){
						displayTime();
						lcd_gotoxy(7,2);
						lcd_print("^^");
						while(PIND==0b00000011);
						if((PIND & (1<<PIND1))==0){
							if(seconds_counter==59){
								seconds_counter = 0;
							}
							else{
								seconds_counter++;
							}
							
						}
						else
						{
							time_settings_menu = setTime_set_time_highlighted;
						}
						_delay_ms(300);
					}
					clear_lcd();
					while(time_settings_menu==setTime_set_time_highlighted && menu_selected==setTime_menu){
						displayTime();
						lcd_gotoxy(10,2);
						lcd_print("^^");
						while(PIND==0b00000011);
						if((PIND & (1<<PIND1))==0){
							menu_selected = main_menu_selected;
							//TIMSK = 1<<OCIE1A;
						}
						else{
							time_settings_menu = setTime_hour_highlighted;
						}
					}
					_delay_ms(300);
					clear_lcd();
				}
				TIMSK |= 1<<OCIE1A;
			}
			/////////////////////////////////////////////////////////////////////
			//else if((PIND & (1<<PIND0))==0){
			else{
				clear_lcd();
				lcd_gotoxy(1,1);
				lcd_print("Click to update");
				lcd_gotoxy(1,2);
				lcd_print("Alarm 1");
				_delay_ms(300);
				////////////////////////////////////////////////////////////////////
				// Set Alarm 1 Menu Option
				////////////////////////////////////////////////////////////////////
				while(PIND==0b00000011);
				//_delay_ms(300);
				if((PIND & (1<<PIND1))==0){
					_delay_ms(300);
					clear_lcd();
					menu_selected = alarm_1_menu;
					alarm_1_settings_menu = alarm_1_hour_highlighted;
					while(menu_selected==alarm_1_menu){
						while(alarm_1_settings_menu==alarm_1_hour_highlighted){
							display_alarm_1_time();
							lcd_gotoxy(1,2);
							lcd_print("^^");
							while(PIND==0b00000011);
							if((PIND & (1<<PIND1))==0){
								if(alarm_1_hour_counter==23){
									alarm_1_hour_counter = 0;
								}
								else{
									alarm_1_hour_counter++;
								}
								current_alarm_1_hour_counter = alarm_1_hour_counter;
							}
							else{
								alarm_1_settings_menu = alarm_1_minute_highlighted;
							}
							_delay_ms(300);
						}
						clear_lcd();
						while(alarm_1_settings_menu==alarm_1_minute_highlighted){
							display_alarm_1_time();
							lcd_gotoxy(4,2);
							lcd_print("^^");
							while(PIND==0b00000011);
							if((PIND & (1<<PIND1))==0){
								if(alarm_1_minute_counter==59){
									alarm_1_minute_counter = 0;
								}
								else{
									alarm_1_minute_counter++;
								}
								current_alarm_1_minute_counter = alarm_1_minute_counter;
							}
							else{
								alarm_1_settings_menu = alarm_1_set_highlighted;
							}
							_delay_ms(300);
						}
						clear_lcd();
						while(alarm_1_settings_menu==alarm_1_set_highlighted && menu_selected==alarm_1_menu){
							display_alarm_1_time();
							lcd_gotoxy(7,2);
							lcd_print("^^");
							while(PIND==0b00000011);
							if((PIND & (1<<PIND1))==0){
								menu_selected = main_menu_selected;
								status_of_alarm_1 = active;
								//alarm_1_change = 1;
								previous_alarm_1_hour_counter_counter = current_alarm_1_hour_counter;
								previous_alarm_1_minute_counter = current_alarm_1_minute_counter;
								clear_lcd();
								lcd_gotoxy(4,1);
								lcd_print("Alarm 1 Set");
							}
							else{
								alarm_1_settings_menu = alarm_1_off_highlighted;
							}
							_delay_ms(300);
						}
						clear_lcd();
						while(alarm_1_settings_menu==alarm_1_off_highlighted && menu_selected==alarm_1_menu){
							display_alarm_1_time();
							lcd_gotoxy(14,2);
							lcd_print("^^");
							while(PIND==0b00000011);
							if((PIND & (1<<PIND1))==0){
								menu_selected = main_menu_selected;
								//alarm_1_change = 0;
								status_of_alarm_1 = deactive;
								clear_lcd();
								lcd_gotoxy(4,1);
								lcd_print("Alarm 1 Off");
							}
							else{
								alarm_1_settings_menu = alarm_1_hour_highlighted;
							}
							_delay_ms(300);
						}
						clear_lcd();
					}
				}
				//////////////////////////////////////////////////////////////////////////
				else if((PIND & (1<<PIND0))==0){
					clear_lcd();
					lcd_gotoxy(1,1);
					lcd_print("Click to update");
					lcd_gotoxy(1,2);
					lcd_print("Alarm 2");
					_delay_ms(300);
					while(PIND==0b00000011);
					
					if((PIND & (1<<PIND1))==0){
						_delay_ms(300);
						clear_lcd();
						menu_selected = alarm_2_menu;
						alarm_2_settings_menu = alarm_2_hour_highlighted;
						while(menu_selected==alarm_2_menu){
							while(alarm_2_settings_menu==alarm_2_hour_highlighted){
								display_alarm_2_time();
								lcd_gotoxy(1,2);
								lcd_print("^^");
								while(PIND==0b00000011);
								if((PIND & (1<<PIND1))==0){
									if(alarm_2_hour_counter==23){
										alarm_2_hour_counter = 0;
									}
									else{
										alarm_2_hour_counter++;
									}
									current_alarm_2_hour_counter = alarm_2_hour_counter;
								}
								else{
									alarm_2_settings_menu = alarm_2_minute_highlighted;
								}
								_delay_ms(300);
							}
							clear_lcd();
							while(alarm_2_settings_menu==alarm_2_minute_highlighted){
								display_alarm_2_time();
								lcd_gotoxy(4,2);
								lcd_print("^^");
								while(PIND==0b00000011);
								if((PIND & (1<<PIND1))==0){
									if(alarm_2_minute_counter==59){
										alarm_2_minute_counter = 0;
									}
									else{
										alarm_2_minute_counter++;
									}
									current_alarm_2_minute_counter = alarm_2_minute_counter;
								}
								else{
									alarm_2_settings_menu = alarm_2_set_highlighted;
								}
								_delay_ms(300);
							}
							clear_lcd();
							while(alarm_2_settings_menu==alarm_2_set_highlighted && menu_selected==alarm_2_menu){
								display_alarm_2_time();
								lcd_gotoxy(7,2);
								lcd_print("^^");
								while(PIND==0b00000011);
								if((PIND & (1<<PIND1))==0){
									menu_selected = main_menu_selected;
									//alarm_2_change = 1;
									status_of_alarm_2 = active;
									previous_alarm_2_hour_counter_counter = current_alarm_2_hour_counter;
									previous_alarm_2_minute_counter = current_alarm_2_minute_counter;
									clear_lcd();
									lcd_gotoxy(4,1);
									lcd_print("Alarm 2 Set");
								}
								else{
									alarm_2_settings_menu = alarm_2_off_highlighted;
								}
								_delay_ms(300);
							}
							clear_lcd();
							while(alarm_2_settings_menu==alarm_2_off_highlighted && menu_selected==alarm_2_menu){
								display_alarm_2_time();
								lcd_gotoxy(14,2);
								lcd_print("^^");
								while(PIND==0b00000011);
								if((PIND & (1<<PIND1))==0){
									menu_selected = main_menu_selected;
									//alarm_2_change = 0;
									status_of_alarm_2 = deactive;
									clear_lcd();
									lcd_gotoxy(4,1);
									lcd_print("Alarm 2 Off");
								}
								else{
									alarm_2_settings_menu = alarm_2_hour_highlighted;
								}
								_delay_ms(300);
							}
							clear_lcd();
						}
					}
					/////////////////////////////////////////////////////////////////////////
				}
			}
			
			
		//	_delay_ms(300);
		//	clear_lcd();
		}
		else{
			beeping_alarm();
			/*
			if((PIND & (1<<PIND0))==0 || (PIND & (1<<PIND1))==0){
				if(alarm_1_state==on){
					status_of_alarm_1 = deactive;
					alarm_1_state = off;
					PORTB |= ~(1<<PORTB0);
				}
			    if(alarm_2_state==on){
					status_of_alarm_2 = deactive;
					alarm_2_state = off;
					PORTB |= ~(1<<PORTB0);
				}
			}
			*/
		}
		_delay_ms(300);
		clear_lcd();
		
	}
		
}
void init_ports(){
	DDRD &= ~((1<<PIND0)|(1<<PIND1));      // Configure PD2 and PD3 to be input pins
	DDRB = 0xFF;                       // Configure PortB to be an output port
	PORTD = ((1<<PIND0)|(1<<PIND1));      // Activate internal pull-up resistor
}

void init_timers(){
	TIMSK |= (1<<OCIE1A);       // Enables output compare 1 match A interrupt
	TCCR1A = 0;                 // sets the timer/counter1 control compare match A register to 0
	TCCR1B = 0b00001100;          // CTC mode with a pre-scaler of 256
	OCR1A = 31250;              // clock cycles required to generate a delay of 1 seconds
}

ISR(TIMER1_COMPA_vect){
	seconds_counter++;
	if(seconds_counter==60){
		seconds_counter = 0;
		minutes_counter++;
	}
	if(minutes_counter==60){
		minutes_counter = 0;
		hours_counter++;
	}
	
	if(hours_counter==24){
		hours_counter = 0;
	}
	
	if(status_of_alarm_1==active){
		if(previous_alarm_1_hour_counter_counter==hours_counter && previous_alarm_1_minute_counter==minutes_counter && alarm_1_state==off){
			alarm_1_state = on;
			//status_of_alarm_1 = deactive;
			//PORTB |= (1<<PORTB0); 
		}
	}
	if(status_of_alarm_2==active){
		if(previous_alarm_2_hour_counter_counter==hours_counter && previous_alarm_2_minute_counter==minutes_counter && alarm_2_state==off){
			alarm_2_state = on;
			//status_of_alarm_1 = deactive;
			//PORTB |= (1<<PORTB0);
		}
	}
	///////////////////////////////////////////////////////////////////
	if(alarm_1_state==on){
		seconds_alarm_1_on++;
		if(seconds_alarm_1_on==60){
			seconds_alarm_1_on = 0;
			alarm_1_beeping_time = sixty_seconds_reached;
		}
		else{
			alarm_1_beeping_time = sixty_seconds_not_reached;
		}
	}
	else{
		seconds_alarm_1_on = 0;
		alarm_1_beeping_time = sixty_seconds_not_reached;
	}
	
	if(alarm_2_state==on){
		seconds_alarm_2_on++;
		if(seconds_alarm_2_on==60){
			seconds_alarm_2_on = 0;
			alarm_2_beeping_time = sixty_seconds_reached;
		}
		else{
			alarm_2_beeping_time = sixty_seconds_not_reached;
		}
	}
	else{
		seconds_alarm_2_on = 0;
		alarm_2_beeping_time = sixty_seconds_not_reached;
	}
	//////////////////////////////////////////////////////////////////
}

void displayTime(){
	
	// Splitting of values into individual digits
	seconds_zero_val = seconds_counter%10;
	seconds_one_val = seconds_counter/10;
	
	minutes_zero_val = minutes_counter%10;
	minutes_one_val = minutes_counter/10;
	
	hours_zero_val = hours_counter%10;
	hours_one_val = hours_counter/10;
	
	// Conversion of values to ASCII code
	seconds_zero = DISPLAY[seconds_zero_val];
	seconds_one =  DISPLAY[seconds_one_val];
	
	minutes_zero = DISPLAY[minutes_zero_val];
	minutes_one = DISPLAY[minutes_one_val];
	
	hours_zero = DISPLAY[hours_zero_val];
	hours_one = DISPLAY[hours_one_val];
	
	lcd_gotoxy(1,1);
	lcdData(hours_one);
	lcdData(hours_zero);
	lcd_print(":");
	lcdData(minutes_one);
	lcdData(minutes_zero);
	lcd_print(":");
	lcdData(seconds_one);
	lcdData(seconds_zero);
	lcd_gotoxy(10,1);
	lcd_print("SetTime");
	//lcd_print(" SetTime");
}
		
void display_alarm_1_time(){
	alarm_1_hour_one_val = DISPLAY[alarm_1_hour_counter/10];
	alarm_1_hour_zero_val = DISPLAY[alarm_1_hour_counter%10];
	alarm_1_minute_one_val = DISPLAY[alarm_1_minute_counter/10];
	alarm_1_minute_zero_val = DISPLAY[alarm_1_minute_counter%10];
	
	lcd_gotoxy(1,1);
	lcdData(alarm_1_hour_one_val);
	lcdData(alarm_1_hour_zero_val);
	lcd_print(":");
	lcdData(alarm_1_minute_one_val);
	lcdData(alarm_1_minute_zero_val);
	lcd_print(" SetAL1");
	lcd_print(" Off");
	lcd_gotoxy(16,2);
	lcdData(0);
}
void display_alarm_2_time(){
	alarm_2_hour_one_val = DISPLAY[alarm_2_hour_counter/10];
	alarm_2_hour_zero_val = DISPLAY[alarm_2_hour_counter%10];
	alarm_2_minute_one_val = DISPLAY[alarm_2_minute_counter/10];
	alarm_2_minute_zero_val = DISPLAY[alarm_2_minute_counter%10];
	
	lcd_gotoxy(1,1);
	lcdData(alarm_2_hour_one_val);
	lcdData(alarm_2_hour_zero_val);
	lcd_print(":");
	lcdData(alarm_2_minute_one_val);
	lcdData(alarm_2_minute_zero_val);
	lcd_print(" SetAL2");
	lcd_print(" Off");
	lcd_gotoxy(16,2);
	lcdData(0);
}


void beeping_alarm(){
	if(alarm_1_state==on){
		clear_lcd();
		while(((PIND & (1<<PIND0))==1) && (alarm_1_beeping_time==sixty_seconds_not_reached) && (alarm_1_state==on)){
			PORTB ^= (1<<PORTB0);
			lcd_gotoxy(4,1);
			lcd_print("WAKE UP");
			lcd_gotoxy(1,2);
			lcd_print("Press to stop");
			if((PIND & (1<<PIND0))==0){
				alarm_1_state = off;
				status_of_alarm_1 = deactive;
				//PORTB &= ~(1<<PORTB0);
			}
			else if((PIND & (1<<PIND1))==0){
				alarm_1_state = off;
				status_of_alarm_1 = deactive;
				//PORTB &= ~(1<<PORTB0);
			}
			_delay_ms(100);
		}
		clear_lcd();
		alarm_1_state = off;
		status_of_alarm_1 = deactive;
		PORTB &= ~(1<<PORTB0);
	}
	
    if(alarm_2_state==on){
	    clear_lcd();
	    while(((PIND & (1<<PIND0))==1) && (alarm_2_beeping_time==sixty_seconds_not_reached) && (alarm_2_state==on)){
		    PORTB ^= (1<<PORTB0);
		    lcd_gotoxy(4,1);
		    lcd_print("WAKE UP");
		    lcd_gotoxy(1,2);
		    lcd_print("Press to stop");
		    if((PIND & (1<<PIND0))==0){
			    alarm_2_state = off;
				status_of_alarm_2 = deactive;
				//PORTB &= ~(1<<PORTB0);
		    }
			else if((PIND & (1<<PIND1))==0){
				alarm_2_state = off;
				status_of_alarm_2 = deactive;
				//PORTB &= ~(1<<PORTB0);
			}
		    _delay_ms(100);
	    }
	    clear_lcd();
	    alarm_2_state = off;
	    status_of_alarm_2 = deactive;
	   // PORTB |= ~(1<<PORTB0);
    }
	PORTB &= ~(1<<PORTB0);
    _delay_ms(300);
	clear_lcd();
}
