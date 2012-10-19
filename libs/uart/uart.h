/*
 *  uart.h
 *  TEST
 *
 *  Created by Joe  Schaack on 8/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _uart_H_
#define _uart_H_

#define TX_BUFFER_SIZE 0
#define RX_BUFFER_SIZE 0

void uartInit(uint32_t baudrate);
void uartSend(char data);
uint8_t uartDataAvailable();
uint8_t uartRead();

#define SCB_SYSAHBCLKCTRL_UART                    ((unsigned int) 0x00001000) // Enables clock for UART.  UART pins must be configured

#define SCB_UARTCLKDIV_DIV1                       ((unsigned int) 0x00000001) // Divide UART clock by 1 (can be set from 1..255)


#define IOCON_PIO1_6_FUNC_MASK                    ((unsigned int) 0x00000007)
#define IOCON_PIO1_6_FUNC_GPIO                    ((unsigned int) 0x00000000)
#define IOCON_PIO1_6_FUNC_UART_RXD                ((unsigned int) 0x00000001)
#define IOCON_PIO1_6_FUNC_CT32B0_MAT0             ((unsigned int) 0x00000002)
#define IOCON_PIO1_6_MODE_MASK                    ((unsigned int) 0x00000018)
#define IOCON_PIO1_6_MODE_INACTIVE                ((unsigned int) 0x00000000)
#define IOCON_PIO1_6_MODE_PULLDOWN                ((unsigned int) 0x00000008)
#define IOCON_PIO1_6_MODE_PULLUP                  ((unsigned int) 0x00000010)
#define IOCON_PIO1_6_MODE_REPEATER                ((unsigned int) 0x00000018)
#define IOCON_PIO1_6_HYS_MASK                     ((unsigned int) 0x00000020)
#define IOCON_PIO1_6_HYS_DISABLE                  ((unsigned int) 0x00000000)
#define IOCON_PIO1_6_HYS_ENABLE                   ((unsigned int) 0x00000020)

#define IOCON_PIO1_7_FUNC_MASK                    ((unsigned int) 0x00000007)
#define IOCON_PIO1_7_FUNC_GPIO                    ((unsigned int) 0x00000000)
#define IOCON_PIO1_7_FUNC_UART_TXD                ((unsigned int) 0x00000001)
#define IOCON_PIO1_7_FUNC_CT32B0_MAT1             ((unsigned int) 0x00000002)
#define IOCON_PIO1_7_MODE_MASK                    ((unsigned int) 0x00000018)
#define IOCON_PIO1_7_MODE_INACTIVE                ((unsigned int) 0x00000000)
#define IOCON_PIO1_7_MODE_PULLDOWN                ((unsigned int) 0x00000008)
#define IOCON_PIO1_7_MODE_PULLUP                  ((unsigned int) 0x00000010)
#define IOCON_PIO1_7_MODE_REPEATER                ((unsigned int) 0x00000018)
#define IOCON_PIO1_7_HYS_MASK                     ((unsigned int) 0x00000020)
#define IOCON_PIO1_7_HYS_DISABLE                  ((unsigned int) 0x00000000)
#define IOCON_PIO1_7_HYS_ENABLE                   ((unsigned int) 0x00000020)





/*  Receive buffer */
#define RBR_MASK                           ((unsigned int) 0x000000FF)

/*  Transmitter holding register */

/*  Divisor latch LSB */

/*  Divisor latch MSB */

/* Interrupt enable */
#define IER_RBR_Interrupt_MASK             ((unsigned int) 0x00000001) // Enables the received data available interrupt
#define IER_RBR_Interrupt_Enabled          ((unsigned int) 0x00000001)
#define IER_RBR_Interrupt_Disabled         ((unsigned int) 0x00000000)
#define IER_THRE_Interrupt_MASK            ((unsigned int) 0x00000002) // Enables the THRE interrupt
#define IER_THRE_Interrupt_Enabled         ((unsigned int) 0x00000002)
#define IER_THRE_Interrupt_Disabled        ((unsigned int) 0x00000000)
#define IER_RLS_Interrupt_MASK             ((unsigned int) 0x00000004) // Enables the Rx line status interrupt
#define IER_RLS_Interrupt_Enabled          ((unsigned int) 0x00000004)
#define IER_RLS_Interrupt_Disabled         ((unsigned int) 0x00000000)
#define IER_ABEOIntEn_MASK                 ((unsigned int) 0x00000100) // End of auto-baud interrupt
#define IER_ABEOIntEn_Enabled              ((unsigned int) 0x00000100)
#define IER_ABEOIntEn_Disabled             ((unsigned int) 0x00000000)
#define IER_ABTOIntEn_MASK                 ((unsigned int) 0x00000200) // Auto-baud timeout interrupt
#define IER_ABTOIntEn_Enabled              ((unsigned int) 0x00000200)
#define IER_ABTOIntEn_Disabled             ((unsigned int) 0x00000000)

/*  Interrupt identification */
#define IIR_IntStatus_MASK                 ((unsigned int) 0x00000001) // Interrupt status
#define IIR_IntStatus_InterruptPending     ((unsigned int) 0x00000001)
#define IIR_IntStatus_NoInterruptPending   ((unsigned int) 0x00000000)
#define IIR_IntId_MASK                     ((unsigned int) 0x0000000E) // Interrupt identification
#define IIR_IntId_RLS                      ((unsigned int) 0x00000006) // Receive line status
#define IIR_IntId_RDA                      ((unsigned int) 0x00000004) // Receive data available
#define IIR_IntId_CTI                      ((unsigned int) 0x0000000C) // Character time-out indicator
#define IIR_IntId_THRE                     ((unsigned int) 0x00000002) // THRE interrupt
#define IIR_IntId_MODEM                    ((unsigned int) 0x00000000) // Modem interrupt
#define IIR_FIFO_Enable_MASK               ((unsigned int) 0x000000C0)
#define IIR_ABEOInt_MASK                   ((unsigned int) 0x00000100) // End of auto-baud interrupt
#define IIR_ABEOInt                        ((unsigned int) 0x00000100)
#define IIR_ABTOInt_MASK                   ((unsigned int) 0x00000200) // Auto-baud time-out interrupt
#define IIR_ABTOInt                        ((unsigned int) 0x00000200)

/*  FIFO control */
#define FCR_FIFO_Enable_MASK               ((unsigned int) 0x00000001) // UART FIFOs enabled/disabled
#define FCR_FIFO_Enabled                   ((unsigned int) 0x00000001)
#define FCR_FIFO_Disabled                  ((unsigned int) 0x00000000)
#define FCR_Rx_FIFO_Reset_MASK             ((unsigned int) 0x00000002)
#define FCR_Rx_FIFO_Reset                  ((unsigned int) 0x00000002) // Clear Rx FIFO
#define FCR_Tx_FIFO_Reset_MASK             ((unsigned int) 0x00000004)
#define FCR_Tx_FIFO_Reset                  ((unsigned int) 0x00000004) // Clear Tx FIFO
#define FCR_Rx_Trigger_Level_Select_MASK   ((unsigned int) 0x000000C0) // Chars written before before interrupt
#define FCR_Rx_Trigger_Level_Select_1Char  ((unsigned int) 0x00000000) 
#define FCR_Rx_Trigger_Level_Select_4Char  ((unsigned int) 0x00000040) 
#define FCR_Rx_Trigger_Level_Select_8Char  ((unsigned int) 0x00000080) 
#define FCR_Rx_Trigger_Level_Select_14Char ((unsigned int) 0x000000C0) 

/*  Modem control */
#define MCR_DTR_Control_MASK               ((unsigned int) 0x00000001) // Source for modem output pin DTR
#define MCR_DTR_Control                    ((unsigned int) 0x00000001)
#define MCR_RTS_Control_MASK               ((unsigned int) 0x00000002) // Source for modem output pin RTS
#define MCR_RTS_Control                    ((unsigned int) 0x00000002)
#define MCR_Loopback_Mode_Select_MASK      ((unsigned int) 0x00000010) // Diagnostic loopback mode
#define MCR_Loopback_Mode_Select_Enabled   ((unsigned int) 0x00000010)
#define MCR_Loopback_Mode_Select_Disabled  ((unsigned int) 0x00000000)
#define MCR_RTSen_MASK                     ((unsigned int) 0x00000040) // Disable auto-rts flow control
#define MCR_RTSen_Enabled                  ((unsigned int) 0x00000040)
#define MCR_RTSen_Disabled                 ((unsigned int) 0x00000000)
#define MCR_CTSen_MASK                     ((unsigned int) 0x00000080) // Disable auto-cts flow control
#define MCR_CTSen_Enabled                  ((unsigned int) 0x00000080)
#define MCR_CTSen_Disabled                 ((unsigned int) 0x00000000)

/*  Line control */
#define LCR_Word_Length_Select_MASK        ((unsigned int) 0x00000003) // Word Length Selector
#define LCR_Word_Length_Select_5Chars      ((unsigned int) 0x00000000)
#define LCR_Word_Length_Select_6Chars      ((unsigned int) 0x00000001)
#define LCR_Word_Length_Select_7Chars      ((unsigned int) 0x00000002)
#define LCR_Word_Length_Select_8Chars      ((unsigned int) 0x00000003)
#define LCR_Stop_Bit_Select_MASK           ((unsigned int) 0x00000004) // Stop bit select
#define LCR_Stop_Bit_Select_1Bits          ((unsigned int) 0x00000000)
#define LCR_Stop_Bit_Select_2Bits          ((unsigned int) 0x00000004)
#define LCR_Parity_Enable_MASK             ((unsigned int) 0x00000008) // Parity enable
#define LCR_Parity_Enabled                 ((unsigned int) 0x00000008)
#define LCR_Parity_Disabled                ((unsigned int) 0x00000000)
#define LCR_Parity_Select_MASK             ((unsigned int) 0x00000030) // Parity select
#define LCR_Parity_Select_OddParity        ((unsigned int) 0x00000000)
#define LCR_Parity_Select_EvenParity       ((unsigned int) 0x00000010)
#define LCR_Parity_Select_Forced1          ((unsigned int) 0x00000020)
#define LCR_Parity_Select_Forced0          ((unsigned int) 0x00000030)
#define LCR_Break_Control_MASK             ((unsigned int) 0x00000040) // Break transmission control
#define LCR_Break_Control_Enabled          ((unsigned int) 0x00000040)
#define LCR_Break_Control_Disabled         ((unsigned int) 0x00000000)
#define LCR_Divisor_Latch_Access_MASK      ((unsigned int) 0x00000080) // Divisor latch access
#define LCR_Divisor_Latch_Access_Enabled   ((unsigned int) 0x00000080)
#define LCR_Divisor_Latch_Access_Disabled  ((unsigned int) 0x00000000)

/*  Line status */
#define LSR_RDR_MASK                       ((unsigned int) 0x00000001) // Receiver data ready
#define LSR_RDR_EMPTY                      ((unsigned int) 0x00000000) // U0RBR is empty
#define LSR_RDR_DATA                       ((unsigned int) 0x00000001) // U0RBR contains valid data
#define LSR_OE_MASK                        ((unsigned int) 0x00000002) // Overrun error
#define LSR_OE                             ((unsigned int) 0x00000002)
#define LSR_PE_MASK                        ((unsigned int) 0x00000004) // Parity error
#define LSR_PE                             ((unsigned int) 0x00000004)
#define LSR_FE_MASK                        ((unsigned int) 0x00000008) // Framing error
#define LSR_FE                             ((unsigned int) 0x00000008)
#define LSR_BI_MASK                        ((unsigned int) 0x00000010) // Break interrupt
#define LSR_BI                             ((unsigned int) 0x00000010)
#define LSR_THRE_MASK                      ((unsigned int) 0x00000020) // Transmitter holding register empty
#define LSR_THRE                           ((unsigned int) 0x00000020)
#define LSR_TEMT_MASK                      ((unsigned int) 0x00000040) // Transmitter empty
#define LSR_TEMT                           ((unsigned int) 0x00000040)
#define LSR_RXFE_MASK                      ((unsigned int) 0x00000080) // Error in Rx FIFO
#define LSR_RXFE                           ((unsigned int) 0x00000080)

/*  Modem status */
#define MSR_Delta_CTS_MASK                 ((unsigned int) 0x00000001) // State change of input CTS
#define MSR_Delta_CTS                      ((unsigned int) 0x00000001)
#define MSR_Delta_DSR_MASK                 ((unsigned int) 0x00000002) // State change of input DSR
#define MSR_Delta_DSR                      ((unsigned int) 0x00000002)
#define MSR_Trailing_Edge_RI_MASK          ((unsigned int) 0x00000004) // Low to high transition of input RI
#define MSR_Trailing_Edge_RI               ((unsigned int) 0x00000004)
#define MSR_Delta_DCD_MASK                 ((unsigned int) 0x00000008) // State change of input DCD
#define MSR_Delta_DCD                      ((unsigned int) 0x00000008)
#define MSR_CTS_MASK                       ((unsigned int) 0x00000010) // Clear to send state
#define MSR_CTS                            ((unsigned int) 0x00000010)
#define MSR_DSR_MASK                       ((unsigned int) 0x00000020) // Data set ready state
#define MSR_DSR                            ((unsigned int) 0x00000020)
#define MSR_RI_MASK                        ((unsigned int) 0x00000040) // Ring indicator state
#define MSR_RI                             ((unsigned int) 0x00000040)
#define MSR_DCD_MASK                       ((unsigned int) 0x00000080) // Data carrier detect state
#define MSR_DCD                            ((unsigned int) 0x00000080)

/*  Scratch pad */

/*  Auto-baud control */
#define ACR_Start_MASK                     ((unsigned int) 0x00000001) // Auto-baud start/stop
#define ACR_Start                          ((unsigned int) 0x00000001)
#define ACR_Stop                           ((unsigned int) 0x00000000)
#define ACR_Mode_MASK                      ((unsigned int) 0x00000002) // Auto-baud mode select
#define ACR_Mode_Mode1                     ((unsigned int) 0x00000000)
#define ACR_Mode_Mode2                     ((unsigned int) 0x00000002)
#define ACR_AutoRestart_MASK               ((unsigned int) 0x00000004)
#define ACR_AutoRestart_NoRestart          ((unsigned int) 0x00000000)
#define ACR_AutoRestart_Restart            ((unsigned int) 0x00000004) // Restart in case of time-out
#define ACR_ABEOIntClr_MASK                ((unsigned int) 0x00000100) // End of auto-baud interrupt clear bit
#define ACR_ABEOIntClr                     ((unsigned int) 0x00000100) 
#define ACR_ABTOIntClr_MASK                ((unsigned int) 0x00000200) // Auto-baud timeout interrupt clear bit
#define ACR_ABTOIntClr                     ((unsigned int) 0x00000200)

/*  Fractional divider */
#define FDR_DIVADDVAL_MASK                 ((unsigned int) 0x0000000F) // Fractional divider: prescaler register
#define FDR_MULVAL_MASK                    ((unsigned int) 0x000000F0) // Fractional divider: prescaler multiplier

/*  Transmit enable */
#define TER_TXEN_MASK                      ((unsigned int) 0x00000080) // UART transmit enable
#define TER_TXEN_Enabled                   ((unsigned int) 0x00000080)
#define TER_TXEN_Disabled                  ((unsigned int) 0x00000000)

/*  RS485 control register */
#define RS485CTRL_NMMEN_MASK               ((unsigned int) 0x00000001) // Normal multi-drop mode
#define RS485CTRL_NMMEN                    ((unsigned int) 0x00000001)
#define RS485CTRL_RXDIS_MASK               ((unsigned int) 0x00000002) // Receiver
#define RS485CTRL_RXDIS                    ((unsigned int) 0x00000002)
#define RS485CTRL_AADEN_MASK               ((unsigned int) 0x00000004) // Auto-address detect
#define RS485CTRL_AADEN                    ((unsigned int) 0x00000004)
#define RS485CTRL_SEL_MASK                 ((unsigned int) 0x00000008) 
#define RS485CTRL_SEL_RTS                  ((unsigned int) 0x00000000) // Use RTS for direction control
#define RS485CTRL_SEL_DTS                  ((unsigned int) 0x00000008) // Use DTS for direction control
#define RS485CTRL_DCTRL_MASK               ((unsigned int) 0x00000010) // Enable/Disable auto-direction control
#define RS485CTRL_DCTRL_Disabled           ((unsigned int) 0x00000000)
#define RS485CTRL_DCTRL_Enabled            ((unsigned int) 0x00000010)
#define RS485CTRL_OINV_MASK                ((unsigned int) 0x00000020) // Reverse polarity of direction control signal on RTS/DTR pin
#define RS485CTRL_OINV_Normal              ((unsigned int) 0x00000000)
#define RS485CTRL_OINV_Inverted            ((unsigned int) 0x00000020)

/*  RS485 address match */

/*  RS485 delay value */

/*  UART FIFO level */
#define FIFOLVL_RXFIFOLVL_MASK             ((unsigned int) 0x0000000F)
#define FIFOLVL_RXFIFOLVL_Empty            ((unsigned int) 0x00000000)
#define FIFOLVL_RXFIFOLVL_Full             ((unsigned int) 0x0000000F)
#define FIFOLVL_TXFIFOLVL_MASK             ((unsigned int) 0x00000F00)
#define FIFOLVL_TXFIFOLVL_Empty            ((unsigned int) 0x00000000)
#define FIFOLVL_TXFIFOLVL_Full             ((unsigned int) 0x00000F00)







#endif

