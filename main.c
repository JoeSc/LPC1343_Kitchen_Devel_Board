#include <stdint.h>
#include "stdio.c"
#include "LPC13xx.h"

  volatile unsigned int lastTick;
  #include "usbcdc/usb.h"
  #include "usbcdc/usbcore.h"
  #include "usbcdc/usbhw.h"
  #include "usbcdc/cdcuser.h"
  #include "usbcdc/cdc_buf.h"


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

      #define CFG_USBCDC_INITTIMEOUT      (5000)


int main(void) {
	LPC_GPIO2->DIR |= (1<<10);
	/* PLL is already setup */
	SystemCoreClockUpdate();
	/* Blue LED Set as output */
	/* Relay EN Set as output */
//	uartInit(115200);
	SysTick_Config(SystemCoreClock/1000);

	delay_ms(1000);


	CDC_Init();                     // Initialise VCOM
    USB_Init();                     // USB Initialization
    USB_Connect(1);              // USB Connect
    // Wait until USB is configured or timeout occurs
    uint32_t usbTimeout = 0; 
    while ( usbTimeout < CFG_USBCDC_INITTIMEOUT / 10 )
    {
      if (USB_Configuration) break;
      delay_ms(10);             // Wait 10ms
      usbTimeout++;
    }

	while(1)
	{
		printf("Hello %d\r\n",msTicks);
		delay_ms(1000);
	}

	while(1);

}


int puts(const char * str)
{
    int i =0;
    if (USB_Configuration) 
    {
      while(*str)
        cdcBufferWrite(*str++);
      // Check if we can flush the buffer now or if we need to wait
      unsigned int currentTick = msTicks;
      if (currentTick != lastTick)
      {
        uint8_t frame[64];
        uint32_t bytesRead = 0;
        while (cdcBufferDataPending())
        {
          // Read up to 64 bytes as long as possible
          bytesRead = cdcBufferReadLen(frame, 64);
          USB_WriteEP (CDC_DEP_IN, frame, bytesRead);
          delay_ms(1);
        }
        lastTick = currentTick;
      }
    }
    return i;
}

