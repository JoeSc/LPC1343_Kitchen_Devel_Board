/*
 *  uart.c
 *  TEST
 *
 *  Created by Joe  Schaack on 8/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "LPC13xx.h"
#include "uart.h"

#if (TX_BUFFER_SIZE > 0 )
uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t tx_buffer_head = 0;
volatile uint8_t tx_buffer_tail = 0;
#endif 

#if (RX_BUFFER_SIZE >0)
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_head =0;
volatile uint8_t rx_buffer_tail = 0;
#endif

void UART_IRQHandler(void)
{
#if ((TX_BUFFER_SIZE > 0) || (RX_BUFFER_SIZE > 0))
    switch(LPC_UART->IIR & IIR_IntId_MASK)
    {
#if (TX_BUFFER_SIZE > 0)
        case IIR_IntId_THRE:
            {
                uint8_t i = 0;
                while(i < 16)
                {
                    uint8_t tail = tx_buffer_tail;
                    uint8_t c = tx_buffer[tail];

                    LPC_UART->THR = c;

                    if (tail == TX_BUFFER_SIZE-1)
                        tail=0;
                    else
                        tail = tail + 1;

                    if (tail == tx_buffer_head)
                    {
                        LPC_UART->IER &= ~IER_THRE_Interrupt_Enabled;//Disable The interrupt
                        tx_buffer_tail = tail;
                        break;
                    }

                    tx_buffer_tail = tail;
                    i++;
                }
            }
            break;            
#endif
#if (RX_BUFFER_SIZE > 0)
        case IIR_IntId_CTI:            
        case IIR_IntId_RDA:
            {
                
                
                while(LPC_UART->LSR & LSR_RDR_DATA)
                {
                    uint8_t c = LPC_UART->RBR;
                    uint8_t head = rx_buffer_head ;
                    if(head == RX_BUFFER_SIZE-1)
                        head = 0;
                    else
                        head = head + 1;
    
                    /*Dont overwrite the buffer !! */
                    if ( head != rx_buffer_tail)
                    {
                        rx_buffer[rx_buffer_head] = c;
                        rx_buffer_head = head;
                    }
                }
                break;
            }
#endif
        default:
            
            break;
    }
#endif
}

void uartInit(uint32_t baudrate)
{
    uint32_t regVal, num;


    NVIC_DisableIRQ(UART_IRQn);

    /* Set 1.6 UART RXD */
    LPC_IOCON->PIO1_6 &= ~IOCON_PIO1_6_FUNC_MASK;
    LPC_IOCON->PIO1_6 |= IOCON_PIO1_6_FUNC_UART_RXD;

    /* Set 1.7 UART TXD */
    LPC_IOCON->PIO1_7 &= ~IOCON_PIO1_7_FUNC_MASK;	
    LPC_IOCON->PIO1_7 |= IOCON_PIO1_7_FUNC_UART_TXD;

    /* Enable UART clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_UART);
    LPC_SYSCON->UARTCLKDIV = SCB_UARTCLKDIV_DIV1;     /* divided by 1 */

    /* 8 bits, no Parity, 1 Stop bit */
    LPC_UART->LCR = (LCR_Word_Length_Select_8Chars |
            LCR_Stop_Bit_Select_1Bits |
            LCR_Parity_Disabled |
            LCR_Parity_Select_OddParity |
            LCR_Break_Control_Disabled |
            LCR_Divisor_Latch_Access_Enabled);

    regVal = LPC_SYSCON->UARTCLKDIV;
    num = (((72000000 * LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/baudrate;

    LPC_UART->DLM = (num & 0xFF00) >> 8;
    LPC_UART->DLL = num & 0xFF;

    //   DLM = 0x00; // for 38400 baud
    //   DLL = 0x51;


    /* Set DLAB back to 0 */
    LPC_UART->LCR = (LCR_Word_Length_Select_8Chars |
            LCR_Stop_Bit_Select_1Bits |
            LCR_Parity_Disabled |
            LCR_Parity_Select_OddParity |
            LCR_Break_Control_Disabled |
            LCR_Divisor_Latch_Access_Disabled);

    /* Enable and reset TX and RX FIFO. 
       Call the RX interrupt when 14 chars are
       in the FIFO */
    LPC_UART->FCR = (FCR_FIFO_Enabled | 
            FCR_Rx_FIFO_Reset | 
            FCR_Tx_FIFO_Reset |
            FCR_Rx_Trigger_Level_Select_14Char); 

    regVal = LPC_UART->LSR;

    while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
    while ( LPC_UART->LSR & LSR_RDR_DATA )
    {
        /* Dump data from RX FIFO */
        regVal = LPC_UART->RBR;
    }
    /* If Using the RX_Buffer then enable the Interrupt
       Else disable it */
#if (RX_BUFFER_SIZE > 0)
    LPC_UART->IER = IER_RBR_Interrupt_Enabled;
#else
    LPC_UART->IER = 0x0;
#endif

    /* If using any buffering be sure to enable the interrupt */
#if (TX_BUFFER_SIZE > 0 ) || (RX_BUFFER_SIZE > 0)
    NVIC_EnableIRQ(UART_IRQn);
#endif

    LPC_UART->TER = TER_TXEN_Enabled;   
#if (TX_BUFFER_SIZE > 0 )
	tx_buffer_head = 0;
	tx_buffer_tail = 0;
#endif	
#if (RX_BUFFER_SIZE > 0 )
	rx_buffer_head = 0;
	rx_buffer_tail = 0;
#endif	
}



/*#######################################################################################
 *
 * TX Section of the code
 *
 *#####################################################################################*/

#if (TX_BUFFER_SIZE > 0 )
void uartSend(char data)
{

    if ( (!(LPC_UART->LSR & LSR_THRE)) || (tx_buffer_head != tx_buffer_tail) )
    {
        uint8_t newhead;
        if (tx_buffer_head == TX_BUFFER_SIZE-1)
            newhead = 0;
        else
            newhead = tx_buffer_head + 1;

        while(newhead == tx_buffer_tail); // make sure a spot is open

        tx_buffer[tx_buffer_head] = data;
        tx_buffer_head = newhead;

        LPC_UART->IER |= IER_THRE_Interrupt_Enabled;
    }
    else
        LPC_UART->THR = data;
}
#else
void uartSend(char data)
{
    // sit and spin untill fifo is empty.  
    while ( !(LPC_UART->LSR & LSR_THRE) );
    LPC_UART->THR = data;
}
#endif



/*#######################################################################################
 *
 * RX Section of the code
 *
 *#####################################################################################*/

#if (RX_BUFFER_SIZE > 0)
/* Unlike other implementations this is a true or false
   Function due to the possibility of chars hiding in 
   the FIFO */
uint8_t uartDataAvailable()
{
    /*Data is available if the head is not equal to the
      tail or if data is waiting in the FIFO */
    return (rx_buffer_head != rx_buffer_tail) || (LPC_UART->LSR & LSR_RDR_DATA);
}
/* returns a char from the buffer first then from the 
   FIFO */
uint8_t uartRead()
{
    if ( !uartDataAvailable() )
        return -1;
    if (rx_buffer_head != rx_buffer_tail)
    {
        uint8_t c = rx_buffer[rx_buffer_tail];
        if (rx_buffer_tail == RX_BUFFER_SIZE-1)
            rx_buffer_tail = 0;
        else 
            rx_buffer_tail++;
        return c;
    }
    else
        return LPC_UART->RBR;
}

#else
uint8_t uartDataAvailable() {
    return LPC_UART->LSR & LSR_RDR_DATA;
}
uint8_t uartRead() {
    return LPC_UART->RBR;
}
#endif

/**************************************************************************/
void __putchar(const char c)
{
    uartSend(c);
}

/**************************************************************************/
int puts(const char * str)
{
    int i =0;
    while(*str)
    {
        __putchar(*str++);
        i++;
    }
    return i;
}






