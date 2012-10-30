#include <stdint.h>
#include "LPC13xx.h"
#include "stdio.c"
#include "uart.h"
#include "filt_adc.h" 
#include "pwm.h" 
#include "h2rgb.h"

#define MAX(a,b) (((a)>(b))?(a):(b))
#define WFI asm volatile ("wfi");
#define RAMP_DOWN_TIME_MS (1000)
#define RGB_POWER_OFF_TIME_MS (10*1000)
#define W_POWER_OFF_TIME_MS (5*1000)

#define OFF_SWITCH (2)
#define WW_SWITCH (3)
#define RGB_SWITCH (1)

#define RED_POT (0)
#define GREEN_POT (1)
#define BLUE_POT (2)
#define WW_POT (3)


volatile uint32_t msTicks = 0;
volatile uint32_t ledTicks = 0;
volatile uint32_t ledTick_rate = 500;
void SysTick_Handler(void) {
	msTicks++;
	ledTicks++;
	if (ledTicks > ledTick_rate)
	{
		LPC_GPIO2->DATA &= ~(1<<10);
		if (ledTicks > ledTick_rate + 1)
		{
			ledTicks = 0;
			LPC_GPIO2->DATA |= (1<<10);
		}
	}
}

void delay_ms(uint32_t ms) {
	uint32_t now = msTicks;
	while ((msTicks-now) < ms)
		;
}

void initSwitch()
{
	// Set all 4 as GPI/O
	LPC_IOCON->SWDIO_PIO1_3 = 0x91;
	LPC_IOCON->PIO2_3 = 0x10;
	LPC_IOCON->PIO3_0 = 0x10;
	LPC_IOCON->PIO3_1 = 0x10;
	//No need to touch the DIR reg since it should be IN
}

int8_t getSwitch(int num)
{
	switch (num)
	{
		case 0:
			return (LPC_GPIO1->DATA >> 3 ) & 1;
		break;
		case 1:
			return (LPC_GPIO2->DATA >> 3 ) & 1;
		break;
		case 2:
			return (LPC_GPIO3->DATA >> 0 ) & 1;
		break;
		case 3:
			return (LPC_GPIO3->DATA >> 1 ) & 1;
		break;
	}
	return -1;
}


volatile uint32_t last_motion = 0;
void PIOINT2_IRQHandler(void)
{
	LPC_GPIO2->IC |= (1<<9);
	puts("MOTION HANDLER2!\n");
	last_motion = msTicks;
}

void transfer_to_sleep()
{
	ledTick_rate = 2000;
	LPC_SYSCON->PDRUNCFG |= (1<<4);
	while (msTicks > (last_motion + 1000))
	{
		WFI;
	}
	ledTick_rate = 500;
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);
}

void shut_off_supply()
{
	LPC_GPIO1->DATA &= ~(1<<5);
	puts("Turing off Supply\n");
}
void turn_on_supply()
{
	LPC_GPIO1->DATA |= (1<<5);
	puts("Turing on Supply\n");
}


// PIO2_9 has the MOTION_DET signal  
// active low in devel board
void setup_GPIO_INT()
{
	LPC_GPIO2->IE |= (1<<9);
	NVIC_EnableIRQ(EINT2_IRQn);
}

int white_on = 0;
int rgb_on = 0;
int ww_sw_asserted = 0;
int rgb_sw_asserted = 0;
int off_sw_asserted = 0;
uint32_t white_scale = 0;
uint32_t rgb_scale = 0;
uint32_t next_run = 0;
uint32_t rgb_mode_3_next_change = 0;
uint32_t rgb_mode_3_color = 0;
int main(void) {
	LPC_GPIO2->DIR |= (1<<10);
	/* PLL is already setup */
	SystemCoreClockUpdate();
	/* Blue LED Set as output */
	/* Relay EN Set as output */
	LPC_GPIO1->DIR |= (1<<5);
	uartInit(115200);
	puts("Sys Initted\n");
	puts("uart Initted\n");
	initSwitch();
	puts("Switches Initted\n");
	SysTick_Config(SystemCoreClock/1000);
	puts("SysTick Initted\n");
	setup_pwm(1000,4092);
	puts("PWM Initted\n");
	setup_GPIO_INT();
	puts("GPIO INT Initted\n");
	filt_adc_init(450);
	delay_ms(500);
	puts("Filt ADC Initted\n");


	while(1)
	{
		while(next_run > msTicks)
			WFI;
		next_run= msTicks + 10;
		if ((msTicks - last_motion ) > 
			(MAX(W_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS,
				RGB_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS ) + 2000))
		{
			shut_off_supply();
			transfer_to_sleep();
			turn_on_supply();
			/* Wait for the power to actually come on before starting the ramp up */
		}

		// WW_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(WW_SWITCH) == 0)
		{
			if(ww_sw_asserted == 5)
			{
				puts("WW SWITCH PRESSED\n");
				white_on = !white_on;
				white_scale = 0;
			}
			ww_sw_asserted += 1;
		}
		else
			ww_sw_asserted = 0;

		// OFF_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(OFF_SWITCH) == 0)
		{
			if(off_sw_asserted == 5)
			{
				puts("OFF SWITCH PRESSED\n");
				white_on = 0;
				white_scale = 0;
				rgb_on = 0;
				rgb_scale = 0;
			}
			off_sw_asserted += 1;
		}
		else
			off_sw_asserted = 0;
		
		// RGB_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(RGB_SWITCH) == 0)
		{
			if(rgb_sw_asserted == 5)
			{
				puts("RGB SWITCH PRESSED\n");
				rgb_on = !rgb_on;
				rgb_scale = 0;
			}
			if(rgb_sw_asserted == 100)
			{
				puts("RGB SWITCH HELD\n");
				rgb_on = 2;
			}
			if(rgb_sw_asserted == 500)
			{
				puts("RGB SWITCH LONG HELD\n");
				rgb_on = 3;
			}
			rgb_sw_asserted += 1;
		}
		else
			rgb_sw_asserted = 0;

/* This handles the WW LEDs */
		if (((msTicks - last_motion ) > W_POWER_OFF_TIME_MS) && white_on)
		{
			white_scale = RAMP_DOWN_TIME_MS - ((msTicks - last_motion) - W_POWER_OFF_TIME_MS);
			if ((msTicks - last_motion) > (W_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS))
				white_scale = 0;
		}
		if ( white_scale < RAMP_DOWN_TIME_MS && white_on && ((msTicks - last_motion ) < W_POWER_OFF_TIME_MS))
		{
			white_scale = msTicks - last_motion;
			/* Check just in case we get held up and skip the 1000th ms after motion */
			if ( white_scale > RAMP_DOWN_TIME_MS)
				white_scale = RAMP_DOWN_TIME_MS;
		}
		if (white_scale != RAMP_DOWN_TIME_MS)
		{
			setWW( (white_scale * getADCVal(WW_POT)) / RAMP_DOWN_TIME_MS );
			setWW2( (white_scale * getADCVal(WW_POT)) / RAMP_DOWN_TIME_MS );
		}
		else
		{
			setWW(getADCVal(WW_POT));
			setWW2(getADCVal(WW_POT));
		}
		
			

		/* This handles the RGB LEDs */
		if (((msTicks - last_motion ) > RGB_POWER_OFF_TIME_MS) && rgb_on)
		{
			rgb_scale = RAMP_DOWN_TIME_MS - ((msTicks - last_motion) - RGB_POWER_OFF_TIME_MS);
			if ((msTicks - last_motion) > (RGB_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS))
				rgb_scale = 0;
		}
		if ( rgb_scale < RAMP_DOWN_TIME_MS && rgb_on && ((msTicks - last_motion ) < RGB_POWER_OFF_TIME_MS))
		{
			rgb_scale = msTicks - last_motion;
			/* Check just in case we get held up and skip the 1000th ms after motion */
			if ( rgb_scale > RAMP_DOWN_TIME_MS)
				rgb_scale = RAMP_DOWN_TIME_MS;
		}

		if (rgb_on == 1 || rgb_on == 0)
		{
			if (rgb_scale != RAMP_DOWN_TIME_MS)
				setRGB((rgb_scale * getADCVal(RED_POT)) / RAMP_DOWN_TIME_MS,
						(rgb_scale * getADCVal(GREEN_POT)) / RAMP_DOWN_TIME_MS,
						(rgb_scale * getADCVal(BLUE_POT)) / RAMP_DOWN_TIME_MS );
			else
				setRGB(getADCVal(RED_POT), getADCVal(GREEN_POT), getADCVal(BLUE_POT));
		}
		else if (rgb_on == 2)
		{
			uint32_t R,G,B;
			h2rgb((msTicks)%(H2RGB_OUT_RANGE*6), &R, &G, &B);
			if (rgb_scale != RAMP_DOWN_TIME_MS)
				setRGB((rgb_scale * R) / RAMP_DOWN_TIME_MS,
						(rgb_scale * G) / RAMP_DOWN_TIME_MS,
						(rgb_scale * B) / RAMP_DOWN_TIME_MS );
			else
				setRGB(R, G, B);
		}
		else if (rgb_on == 3)
		{
			uint32_t R,G,B;
			if (rgb_mode_3_next_change < msTicks)
			{
				rgb_mode_3_next_change = msTicks + (getADCVal(RED_POT)/4);
				rgb_mode_3_color += 2;
				if(rgb_mode_3_color >=6)
					rgb_mode_3_color = 0;

			}
		
			h2rgb((H2RGB_OUT_RANGE * rgb_mode_3_color), &R, &G, &B);
			if (rgb_scale != RAMP_DOWN_TIME_MS)
				setRGB((rgb_scale * R) / RAMP_DOWN_TIME_MS,
						(rgb_scale * G) / RAMP_DOWN_TIME_MS,
						(rgb_scale * B) / RAMP_DOWN_TIME_MS );
			else
				setRGB(R, G, B);
		}




	}
}
#if 0
int i=0;


uint32_t R_val,G_val,B_val,WW_val;
uint32_t next_run = 0;
while(1)
{
	while(next_run > msTicks)
		WFI;
	next_run= msTicks + 10;
	//h2rgb((msTicks/10)%(H2RGB_OUT_RANGE*6), &R_val, &G_val, &B_val);
	//setRGB(R_val,G_val,B_val);

	R_val = getADCVal(0);
	G_val = getADCVal(1);
	B_val = getADCVal(2);
	WW_val = getADCVal(3);

	setRGB(R_val,G_val,B_val);
	setWW(WW_val);
	setWW2(WW_val);

		printf("%4d %4d %4d %4d %d\n",R_val,G_val,B_val,WW_val,i++);


}

int R,G,B,WW;

while(1)
{
	R = getADCVal(0);
	G = getADCVal(1);
	B = getADCVal(2);
	WW = getADCVal(3);

	setRGB(R,G,B);
	setWW(WW);
	setWW2(WW);
		//printf("%4d %4d %4d %4d %d\n",R,G,B,WW,i++);
}
//- 
//- 
//- 	ADCInit( ADC_CLK );
//- 	puts("ADC INITTED\n");
//- 
//- 
//-  	LPC_GPIO0->DIR |= ((1<<8) | (1<<9));
//- 	LPC_GPIO0->DATA &= ~((1<<8) | (1<<9));
//- 	delay_ms(500);
//- 	
//- 
//- 	while (1) {
//- 		R = ADCRead(0);
//- 		G = ADCRead(1);
//- 		B = ADCRead(2);
//- 		setRGB(R, G, B);
//- 		printf("%4d %4d %4d %d\n",R,G,B,i++);
//- 	}
//- while(1);



// 	LPC_GPIO2->DIR = (1<<10);	
// 
// 	// RGB-LED Test
// 	LPC_GPIO1->DIR |= (1<<10) | (1<<4);	
// 	LPC_IOCON->SWCLK_PIO0_10 = 0x1;
// 	LPC_GPIO0->DIR |= (1<<10);	
// 	LPC_GPIO0->DATA &= ~((1<<10));
// 	LPC_GPIO1->DATA &= ~((1<<10) | (1<<4));
// 
// 	// WW Tets
// 	LPC_GPIO0->DATA &= ~((1<<8) | (1<<9));
// 
// 	// RELAY_EN Test
// 	LPC_GPIO1->DIR |= (1<<5);
// 	LPC_GPIO1->DATA |= (1<<5);

//	ADCInit( ADC_CLK );
//	puts("ADC INITTED\n");
//	sw_init();


//	while (1) {
//		LPC_GPIO2->DATA |= (1<<10);
//	//	rand_delay();
//		delay_ms(500);
//		LPC_GPIO2->DATA &= ~(1<<10);
//	//	rand_delay();
//		delay_ms(500);	
//		//UARTSendChar('A');
//	printf("ADC0 = %d\n",ADCRead(0));
//	printf("ADC1 = %d\n",ADCRead(1));
//	printf("ADC2 = %d\n",ADCRead(2));
//	printf("ADC3 = %d\n",ADCRead(3));
//	printf("sw1 = %d\n",sw_read(1));
//	printf("sw2 = %d\n",sw_read(2));
//	printf("sw3 = %d\n",sw_read(3));
//	printf("sw4 = %d\n",sw_read(4));
//	printf("mot = %d\n",sw_read(5));
//		
//	}
}



// void __putchar(const char c)
// {
// 	UARTSendChar(c);
// }
// int puts(const char * str)
// {
//     int i =0;
//     while(*str)
//     {
//         __putchar(*str++);
//         i++;
//     }
//     return i;
// }
#endif 
