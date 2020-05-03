//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "serial_debug.h"

static bool Rx_Interrupts_Enabled = false;
static bool Tx_Interrupts_Enabled = false;


PC_Buffer UART0_Tx_Buffer;
PC_Buffer UART0_Rx_Buffer;


/****************************************************************************
 *
 ****************************************************************************/
int serial_debug_rx(PC_Buffer *rx_buffer, bool block)
{
   int c;

   while (pc_buffer_empty(rx_buffer))
   {
      if (!block)
         return -1;
   }

   DisableInterrupts();
   pc_buffer_remove(rx_buffer, (char *)&c);
   EnableInterrupts();
   
   return c;
}

/****************************************************************************
 *
 ****************************************************************************/
void serial_debug_tx(uint32_t uart_base, PC_Buffer *tx_buffer, int data)
{
  UART0_Type *uart = (UART0_Type *)(uart_base);
  
  // If there is sapce in the hardwere FIFO, and the circular
  // Queue is empty, send the data to the FIFO.
  if( pc_buffer_empty(tx_buffer) && !(uart->FR & UART_FR_TXFF) )
  {
    uart->DR =   data;
  }
  else
  {
    // Test to see if the circular buffer is full
    // If it is, we wait until there is space.
    while( pc_buffer_full(tx_buffer))
    {
        // wait
    }

    DisableInterrupts();
    // Add the character to the circular buffer
    pc_buffer_add(tx_buffer, (char) data);
    EnableInterrupts();
  }

  // If you're in this function, you want to send data
  // so enable TX interrupts even if they are already enabled.
  uart->IM |= UART_IM_TXIM;

  return;
}


//****************************************************************************
//  This function is called from MicroLIB's stdio library.  By implementing
//  this function, MicroLIB's getchar(), scanf(), etc will now work.
// ****************************************************************************/
int fgetc(FILE* stream)
{
   char c;

   
   if ( Rx_Interrupts_Enabled)
   {
    c = serial_debug_rx(SERIAL_DEBUG_RX_BUF_PTR, true);
   }
   else
   {
     c = uart_rx_poll(SERIAL_DEBUG_UART_BASE,true);
   }

   if (c == '\r')
      c = '\n';

   fputc(c, stdout);

   return c;
}

//****************************************************************************
// This function is called from MicroLIB's stdio library.  By implementing
// this function, MicroLIB's putchar(), puts(), printf(), etc will now work.
// ****************************************************************************/
int fputc(int c, FILE* stream)
{

   if ( Tx_Interrupts_Enabled)
   {
      serial_debug_tx(SERIAL_DEBUG_UART_BASE, SERIAL_DEBUG_TX_BUF_PTR, c);
   }
   else
   {
      uart_tx_poll(SERIAL_DEBUG_UART_BASE, c);
   }

   if (c == '\n')
   {
      if ( Tx_Interrupts_Enabled)
      {
        serial_debug_tx(SERIAL_DEBUG_UART_BASE, SERIAL_DEBUG_TX_BUF_PTR,'\r');
      }
      else
      {
         uart_tx_poll(SERIAL_DEBUG_UART_BASE, '\r');
      }
   }

   return c;
}


//************************************************************************
// Configures the serial debug interface at 115200.
// UART IRQs can be anbled using the two paramters to the function.
//************************************************************************
bool init_serial_debug(bool enable_rx_irq, bool enable_tx_irq)
{
  
  // Enable the GPIO pins for PA0 and PA1 so they form the UART0 interface.
  
  // Initialize the GPIO pins used for the serial debug interface
  if(gpio_enable_port(SERIAL_DBG_GPIO_BASE) == false)
  {
    return false;
  }
  
  if(gpio_config_digital_enable(SERIAL_DBG_GPIO_BASE, SERIAL_DBG_RX_PIN | SERIAL_DBG_TX_PIN) == false)
  {
    return false;
  }
  
  if(gpio_config_alternate_function(SERIAL_DBG_GPIO_BASE, SERIAL_DBG_RX_PIN | SERIAL_DBG_TX_PIN) == false)
  {
    return false;
  }
  
  if (gpio_config_port_control(SERIAL_DBG_GPIO_BASE, SERIAL_DBG_RX_PCTL_M, SERIAL_DBG_RX_PCTL) == false )
  {
    return false;
  }
  
  if (gpio_config_port_control(SERIAL_DBG_GPIO_BASE, SERIAL_DBG_TX_PCTL_M, SERIAL_DBG_TX_PCTL) == false )
  {
    return false;
  }
  
  Rx_Interrupts_Enabled = enable_rx_irq;
  Tx_Interrupts_Enabled = enable_tx_irq;
  
  // Initialize the circular buffer(s)
	if(Rx_Interrupts_Enabled || Tx_Interrupts_Enabled)
	{
		pc_buffer_init(SERIAL_DEBUG_TX_BUF_PTR, UART_BUFFER_SIZE);
		pc_buffer_init(SERIAL_DEBUG_RX_BUF_PTR, UART_BUFFER_SIZE); 
	}
  
  
  if( uart_init(SERIAL_DEBUG_UART_BASE,enable_rx_irq, enable_tx_irq) == false)
  { 
    return false;
  }
  
  return true;
}


//*****************************************************************************
//*****************************************************************************
// UART ISR Handler
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Rx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART_Rx_Flow(uint32_t uart_base, PC_Buffer *rx_buffer)
{
  UART0_Type *uart = (UART0_Type *)(uart_base);
      
  // Remove entries from the RX FIFO until they are all removed
  while( !(uart->FR & UART_FR_RXFE ))
  {
    if( ! pc_buffer_full(rx_buffer))
    {
      // Add the character to the circular buffer
      pc_buffer_add(rx_buffer,uart->DR);
    }
  }

  // Clear the RX interrupts so it can trigger again when the hardware 
  // FIFO becomes full
  uart->ICR |= (UART_ICR_RXIC | UART_ICR_RTIC);
  
}

//*****************************************************************************
// Tx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART_Tx_Flow(uint32_t uart_base, PC_Buffer *tx_buffer)
{
    UART0_Type *uart = (UART0_Type *)(uart_base);    
    char c;
  
    // Check to see if we have any data in the circular queue
    if( ! pc_buffer_empty(tx_buffer))
    {
        // Move data from the circular queue to the hardware FIFO
        // until the hardware FIFO is full or the circular buffer
        // is empty.
        while( (! pc_buffer_empty(tx_buffer)) && !(uart->FR & UART_FR_TXFF ) )
        {
          pc_buffer_remove(tx_buffer, &c);  
          uart->DR = c;
        }
    }
    else
    {
        // Any data in the hardware FIFO will continue to be transmitted
        // but the TX empty interrupt will not be enabled since there
        // is no data in the circular buffer.

        // Disable the TX interrupts.
        uart->IM &= ~UART_IM_TXIM;
    }
    
    // Clear the TX interrupt so it can trigger again when the hardware
    // FIFO is empty
    uart->ICR |= UART_ICR_TXIC;
}

//*****************************************************************************
// UART0 Interrupt Service handler
//*****************************************************************************
void UART0_Handler(void)
{
    uint32_t  status;

    // Check to see if RXMIS or RTMIS is active
    status = UART0->MIS;

    if ( status & (UART_MIS_RXMIS | UART_MIS_RTMIS ) )
    {
       UART_Rx_Flow(SERIAL_DEBUG_UART_BASE, SERIAL_DEBUG_RX_BUF_PTR);
    }

    // Check the TX interrupts
    if ( status & UART_MIS_TXMIS )
    {
      UART_Tx_Flow(SERIAL_DEBUG_UART_BASE, SERIAL_DEBUG_TX_BUF_PTR);
    }
    
    return;
}
