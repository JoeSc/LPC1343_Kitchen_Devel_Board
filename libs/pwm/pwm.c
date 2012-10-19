


#include "LPC13xx.h"
#include "pwm.h"



// WW_DIM  = 16B0_MAT0
// WW2_DIM = 16B0_MAT1
// RED_DIM = 16B0_MAT2
// GRN_DIM = 16B1_MAT1
// BLU_DIM = 32B1_MAT3

// freq in hz, gran
int pwm_gran;
void setup_pwm(int freq, int granularity)
{
	pwm_gran = granularity;
	// WW_DIM PIO0_8
	LPC_IOCON->PIO0_8 = 0x2;
	// WW2_DIM PIO0_9
	LPC_IOCON->PIO0_9 = 0x2;
	// RED_DIM PIO0_10
	LPC_IOCON->SWCLK_PIO0_10 = 0x3;

	LPC_TMR16B0->TCR = 0x0; /* Disable Counting */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7); /* Enable 16B0 clock */
	
	LPC_TMR16B0->PWMC = 0xf; /* PWM Mode enabled for all 16B0 */
	LPC_TMR16B0->PR = (SystemCoreClock / (freq * granularity)) - 1;

	LPC_TMR16B0->MR3 = granularity-1;
	LPC_TMR16B0->MR2 = granularity;
	LPC_TMR16B0->MR1 = granularity;
	LPC_TMR16B0->MR0 = granularity;

	LPC_TMR16B0->MCR = (1<<10);

	LPC_TMR16B0->TCR = 0x1; /* Enable Counting */
	
	// GRN_DIM PIO1_10
	LPC_IOCON->PIO1_10 = 0x2;

	LPC_TMR16B1->TCR = 0x0; /* Disable Counting */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8); /* Enable 16B1 clock */
	
	LPC_TMR16B1->PWMC = 0xa; /* PWM Mode enabled for all 16B1 */
	LPC_TMR16B1->PR = (SystemCoreClock / (freq * granularity)) - 1;

	LPC_TMR16B1->MR3 = granularity-1;
	LPC_TMR16B1->MR1 = granularity;

	LPC_TMR16B1->MCR = (1<<10);

	LPC_TMR16B1->TCR = 0x1; /* Enable Counting */

	// BLU_DIM PIO1_4
	LPC_IOCON->PIO1_4 = 0x82;

	LPC_TMR32B1->TCR = 0x0; /* Disable Counting */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10); /* Enable 32B1 clock */
	
	LPC_TMR32B1->PWMC = 0xa; /* PWM Mode enabled for all 32B1 */
	LPC_TMR32B1->PR = (SystemCoreClock / (freq * granularity)) - 1;

	LPC_TMR32B1->MR1 = granularity-1;
	LPC_TMR32B1->MR3 = granularity;

	LPC_TMR32B1->MCR = (1<<4);

	LPC_TMR32B1->TCR = 0x1; /* Enable Counting */

}





void setRGB(int R, int G, int B)
{
	LPC_TMR16B0->MR2 = pwm_gran - R;
	LPC_TMR16B1->MR1 = pwm_gran - G;
	LPC_TMR32B1->MR3 = pwm_gran - B;
}

void setWW(int data)
{
	LPC_TMR16B0->MR0 = pwm_gran - data;
}
void setWW2(int data)
{
	LPC_TMR16B0->MR1 = pwm_gran - data;
}
