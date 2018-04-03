#include "mailbox.h"
#include "../common/stdlib.h"
#include "mem.h"
#include "uart.h"

int mailbox_send(mailbox_message_t msg){
  uart_puts("    Sending a message\n");  // Debugging
  mailbox_status_t stat;
  // Make sure we can send mail
  do {
    stat = *MAIL0_STATUS;
  } while (stat.full);
  *MAIL0_WRITE = msg;  // Send the message
  uart_puts("    A message is sent\n");  // Debugging
  return 0;
}

mailbox_message_t mailbox_receive(){
  uart_puts("    Receiving a message\n");  // Debugging
    
  mailbox_status_t stat;
  mailbox_message_t res;
  // Make sure that the message is from the right channel
  do {
    // Make sure there is mail to recieve
    uart_puts("     waiting for the right channel\n");  // Debugging
    do {
      stat = *MAIL0_STATUS;
    } while (stat.empty); 
    res = *MAIL0_READ;   // Get the message
  } while (res.channel != 1);                // We're using channel 1 for RaspPi B+ (model 1)
  uart_puts("    A message is received\n");  // Debugging
  
  return res;
}


