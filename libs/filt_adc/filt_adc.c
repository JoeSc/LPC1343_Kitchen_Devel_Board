/*
   * Free running ADC that constantly accumulates and averages ADC values
   */



#include "LPC13xx.h"			/* LPC13xx Peripheral Registers */
#include "filt_adc.h"

#define ADC_NUM			8			/* for LPC13xx */
#define ADC_CLK			4500000		/* set to 4.5Mhz */

#define ADC_SEL 		0x0F 		/* Select the active ADCs */

#define ADC_FILT_CNT	128
#define ADC_OVER_SMPL	4 			/* 2 bits, 12bit total */

uint16_t ADCVal0[ADC_FILT_CNT];
uint16_t ADCVal1[ADC_FILT_CNT];
uint16_t ADCVal2[ADC_FILT_CNT];
uint16_t ADCVal3[ADC_FILT_CNT];

volatile uint32_t adc_buffer_pos;

void filt_adc_init( uint32_t ADC_Clk )
{
	int div;
  /* Disable Power down bit to the ADC block. */  
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

  /* Enable AHB clock to the ADC. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);

#if (ADC_SEL & 0x1)
	LPC_IOCON->JTAG_TDI_PIO0_11		= 0x02;	// Select AD0 pin function  
#endif
#if (ADC_SEL & 0x2)
	LPC_IOCON->JTAG_TMS_PIO1_0		= 0x02;	// Select AD1 pin function
#endif
#if (ADC_SEL & 0x4)
	LPC_IOCON->JTAG_TDO_PIO1_1		= 0x02;	// Select AD2 pin function
#endif
#if (ADC_SEL & 0x8)
	LPC_IOCON->JTAG_nTRST_PIO1_2	= 0x02;	// Select AD3 pin function
#endif
#if (ADC_SEL & 0x10)
	LPC_IOCON->ARM_SWDIO_PIO1_3		= 0x02;	// Select AD4 pin function
#endif
#if (ADC_SEL & 0x20)
	LPC_IOCON->PIO1_4				= 0x01;	// Select AD5 pin function
#endif
#if (ADC_SEL & 0x40)
	LPC_IOCON->PIO1_10				= 0x01;	// Select AD6 pin function
#endif
#if (ADC_SEL & 0x80)
	LPC_IOCON->PIO1_11				= 0x01;	// Select AD7 pin function
#endif

	div = ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/ADC_Clk-1);
	if (div > 255)
		LPC_ADC->CR = 0xFF << 8;
	else
		LPC_ADC->CR = div << 8;
//LPC_ADC->CR = (((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/ADC_Clk-1) & 0xFF )<<8;

LPC_ADC->INTEN = 0x8; /* Interrupt on the third so that the rest should be done */
LPC_ADC->CR |= ( (0<<24) | (1<<16) | (0x0F));
adc_buffer_pos = 0;

 	

NVIC_EnableIRQ(ADC_IRQn);

//printf("AD0CR = %08X\n",LPC_ADC->CR);
//printf("AD0GDR = %08X\n",LPC_ADC->GDR);
//printf("AD0INTEN = %08X\n",LPC_ADC->INTEN);
//printf("AD0DR0 = %08X\n",LPC_ADC->DR0);
//printf("AD0DR1 = %08X\n",LPC_ADC->DR1);
//printf("AD0DR2 = %08X\n",LPC_ADC->DR2);
//printf("AD0DR3 = %08X\n",LPC_ADC->DR3);
//printf("AD0DR4 = %08X\n",LPC_ADC->DR4);
//printf("AD0DR5 = %08X\n",LPC_ADC->DR5);
//printf("AD0DR6 = %08X\n",LPC_ADC->DR6);
//printf("AD0DR7 = %08X\n",LPC_ADC->DR7);
//printf("AD0STAT = %08X\n",LPC_ADC->STAT);



}
void ADC_IRQHandler(void)
{
	//volatile uint32_t reg;
	//reg = LPC_ADC->STAT;
	LPC_ADC->STAT;
	ADCVal0[adc_buffer_pos] = (LPC_ADC->DR0 >> 6 ) & 0x3FF;
	ADCVal1[adc_buffer_pos] = (LPC_ADC->DR1 >> 6 ) & 0x3FF;
	ADCVal2[adc_buffer_pos] = (LPC_ADC->DR2 >> 6 ) & 0x3FF;
	ADCVal3[adc_buffer_pos] = (LPC_ADC->DR3 >> 6 ) & 0x3FF;
	if( ++adc_buffer_pos == (ADC_FILT_CNT))
		adc_buffer_pos = 0;
}



uint32_t getADCVal(int chan)
{
	int i;
	uint32_t avg = 0;
	switch (chan)
	{
		case 0:
			for( i = 0; i < ADC_FILT_CNT; i++)
			{
				//printf("num %d = %d\n",i,ADCVal0[i]);
				avg +=ADCVal0[i];
			}
			return avg/(ADC_FILT_CNT/ADC_OVER_SMPL);
		case 1:
			for( i = 0; i < ADC_FILT_CNT; i++)
				avg +=ADCVal1[i];
			return avg/(ADC_FILT_CNT/ADC_OVER_SMPL);
		case 2:
			for( i = 0; i < ADC_FILT_CNT; i++)
				avg +=ADCVal2[i];
			return avg/(ADC_FILT_CNT/ADC_OVER_SMPL);
		case 3:
			for( i = 0; i < ADC_FILT_CNT; i++)
				avg +=ADCVal3[i];
			return avg/(ADC_FILT_CNT/ADC_OVER_SMPL);
	}
	return -1;
}

