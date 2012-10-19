#include <stdint.h>
#include "LPC13xx.h"
#include "stdio.c"
#include "uart.h"
#include "adc.h" 
#include "filt_adc.h" 
#include "pwm.h" 

#include <math.h>







void HardFault_Handler( void )
{
__asm__("TST LR, #4\n\t"
		"ITE EQ\n\t"
		"MRSEQ R0, MSP\n\t"
		"MRSNE R0, PSP\n\t"
		"B hard_fault_handler_c\n\t");
}

void hard_fault_handler_c (unsigned int * hardfault_args)
{
  unsigned int stacked_r0;
  unsigned int stacked_r1;
  unsigned int stacked_r2;
  unsigned int stacked_r3;
  unsigned int stacked_r12;
  unsigned int stacked_lr;
  unsigned int stacked_pc;
  unsigned int stacked_psr;
 
  stacked_r0 = ((unsigned long) hardfault_args[0]);
  stacked_r1 = ((unsigned long) hardfault_args[1]);
  stacked_r2 = ((unsigned long) hardfault_args[2]);
  stacked_r3 = ((unsigned long) hardfault_args[3]);
 
  stacked_r12 = ((unsigned long) hardfault_args[4]);
  stacked_lr = ((unsigned long) hardfault_args[5]);
  stacked_pc = ((unsigned long) hardfault_args[6]);
  stacked_psr = ((unsigned long) hardfault_args[7]);
 
  printf ("\n\n[Hard fault handler - all numbers in hex]\n");
  printf ("R0 = %x\n", stacked_r0);
  printf ("R1 = %x\n", stacked_r1);
  printf ("R2 = %x\n", stacked_r2);
  printf ("R3 = %x\n", stacked_r3);
  printf ("R12 = %x\n", stacked_r12);
  printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
  printf ("PC [R15] = %x  program counter\n", stacked_pc);
  printf ("PSR = %x\n", stacked_psr);
  printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
  printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
  printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
  printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
  printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
  printf ("SCB_SHCSR = %x\n", SCB->SHCSR);
 
  while (1);
}
  





volatile uint32_t msTicks = 0;
volatile uint32_t ledTicks = 0;
void SysTick_Handler(void) {
	msTicks++;
	ledTicks++;
	if (ledTicks > 500)
	{
		LPC_GPIO2->DATA &= ~(1<<10);
		if (ledTicks > 501)
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

void Custom_Init() {
	/* Setup Blue Blink LED */
	LPC_GPIO2->DIR |= (1<<10);
}


void h2rgb(int H, uint8_t *red, uint8_t *green , uint8_t *blue) {
	int i = H - ( H % 40 );
	int f= H - i;
	if((i/40)%2 == 0) f=40-f;
	int n = (6*(40-f)*255)/240;
	switch(i)
	{
		case 240:
		case 0:  
		
			*red = 255;
			*green = n;
			*blue =0;
		break;
		case 40:  
		
			*red = n;
			*green = 255;
			*blue =0;
		break;
		case 80:  
		
			*red = 0;
			*green = 255;
			*blue =n;
		break;
		case 120:  
		
			*red = 0;
			*green = n;
			*blue =255;
		break;
		case 160:  
		
			*red = n;
			*green = 0;
			*blue =255;
		break;
		case 200:  
		
			*red = 255;
			*green = 0;
			*blue =n;
			break;
	}
}


// void sw_init()
// {
// 	// Set all 4 as GPI/O
// 	LPC_IOCON->SWDIO_PIO1_3 = 0x91;
// 	LPC_IOCON->PIO2_3 = 0x10;
// 	LPC_IOCON->PIO3_0 = 0x10;
// 	LPC_IOCON->PIO3_1 = 0x10;
// 	
// 	LPC_IOCON->PIO2_9 = 0x10;
// 	//No need to touch the DIR reg since it should be IN
// }

// int8_t sw_read(int num)
// {
// 	switch (num)
// 	{
// 		case 1:
// 			return (LPC_GPIO1->DATA >> 3 ) & 1;
// 		break;
// 		case 2:
// 			return (LPC_GPIO2->DATA >> 3 ) & 1;
// 		break;
// 		case 3:
// 			return (LPC_GPIO3->DATA >> 0 ) & 1;
// 		break;
// 		case 4:
// 			return (LPC_GPIO3->DATA >> 1 ) & 1;
// 		break;
// 		case 5:
// 			return (LPC_GPIO2->DATA >> 9 ) & 1;
// 		break;
// 
// 	}
// 	return -1;
// }



void MemManage_Handler() {
	puts("MemM\n");
}
void BusFault_Handler() {
	puts("BusF\n");
}
void UsageFault_Handler() {
	puts("UsagF\n");
}

void EINT2_Handler(void)
{
	LPC_GPIO2->IC |= (1<<9);
	puts("MOTION HANDLER2!\n");
}

// PIO2_9 has the MOTION_DET signal  
// active low in devel board
void attach_GPIO_INT()
{
	LPC_GPIO2->IE |= (1<<9);
	NVIC_EnableIRQ(EINT2_IRQn);
}

int main(void) {
	/* PLL is already setup */
	SystemCoreClockUpdate();
	Custom_Init();
	SysTick_Config(SystemCoreClock/1000);
	uartInit(115200);
	puts("Sys Initted\n");
	puts("Custom Initted\n");
	puts("SysTick Initted\n");
	puts("uart Initted\n");
		
	setup_pwm(1000,1023);
	puts("PWM Initted\n");

	attach_GPIO_INT();

		

	filt_adc_init(4500);

int i=0;


uint8_t R_val,G_val,B_val;
while(1)
{
	float val = 120.0 * sin(msTicks/10000.0) + 120.0;
	h2rgb(val, &R_val, &G_val, &B_val);
	setRGB(R_val*4,G_val*4,B_val*4);

		//printf("%4d %4d %4d %4d %d\n",R_val,G_val,B_val,(int)val,i++);


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

