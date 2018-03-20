#include <stdint.h>
#include "kernel/peripheral.h"

#ifndef MAILBOX_H
#define MAILBOX_H

#define MAILBOX_BASE PERIPHERAL_BASE + MAILBOX_OFFSET
#define MAIL0_READ (((mailbox_message_t *)(0x00 + MAILBOX_BASE)))
#define MAIL0_STATUS (((mailbox_status_t *)(0x18 + MAILBOX_BASE)))
#define MAIL0_WRITE (((mailbox_message_t *)(0x20 + MAILBOX_BASE)))

  // Message
typedef struct {
  uint8_t channel: 1; // Mailbox channel 1
  uint32_t data: 28;
} mailbox_message_t;

typedef struct {
    uint32_t reserved: 30;
    uint8_t empty: 1;
    uint8_t full: 1;
} mailbox_status_t;


// Functions

  // Sending a message
int mailbox_send(mailbox_message_t msg); // Sends a message to the hardware

  // Receiving a message
mailbox_message_t mailbox_receive();    // Receives a message (always from channel 1)

#endif
