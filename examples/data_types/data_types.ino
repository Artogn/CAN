#include <SPI.h>
#include <CAN.h>

/* This program demonstrates how to send and receive different
 * data types using different message IDs.  If you are writing
 * a program that sends different types of messages, you can
 * check the message ID to know how to process the data you
 * receive.  */

/* If you don't have two CAN shields, this program can run on
 * just one by setting the mode to CAN_MODE_LOOPBACK instead
 * of CAN_MODE_NORMAL. */

unsigned long last;
unsigned long now;

const int small_message_id = 0xA1;
const int big_message_id = 0xA2;
const int string_message_id = 0xA3;

byte message_to_send;

byte b1;
byte b2;
int i;
long time;
char s[8];
CanMessage sendMessage;
CanMessage receiveMessage;

void setup()
{
  CAN.begin (CAN_SPEED_500000);
  CAN.setMode (CAN_MODE_NORMAL);
  Serial.begin (115200);
  
  last = 0;
  message_to_send = 0;
}

void loop()
{
  /* Send a message every second */
  now = millis ();
  if (now > last + 1000) {
    last = now;

    /* Clear the message before reusing */
    sendMessage.clear ();

    if (message_to_send == 0) {
      sendMessage.id = small_message_id;
      sendMessage.setByteData (3);
    } else if (message_to_send == 1) {
      sendMessage.id = big_message_id;
      sendMessage.setIntData (1234);      
      sendMessage.setByteData (56);      
      sendMessage.setByteData (78);      
      sendMessage.setLongData (now);      
    } else {
      sendMessage.id = string_message_id;
      
      /* Strings have one extra character to indicate their end.
       * See http://arduino.cc/en/Reference/String */
      sendMessage.setData ("Hello", 6);
    }
    
    sendMessage.send();
    
    message_to_send++;

    /* After all three messages have been sent, start over */
    if (message_to_send >= 3)
        message_to_send = 0;
  }

  /* Receive message if available */
  if (CAN.available ()) {
    receiveMessage = CAN.getMessage ();
    
    /* Process different data types based on message ID */
    if (receiveMessage.id == small_message_id) {
      b1 = receiveMessage.getByteFromData ();
      Serial.println (b1);
    } else if (receiveMessage.id == big_message_id) {
      /* Make sure to read the bytes in the order they were written */
      i = receiveMessage.getIntFromData ();
      b1 = receiveMessage.getByteFromData ();
      b2 = receiveMessage.getByteFromData ();
      time = receiveMessage.getLongFromData ();

      Serial.print (i);
      Serial.print (" ");
      Serial.print (b1);
      Serial.print (" ");
      Serial.print (b2);
      Serial.print (" ");
      Serial.println (time);
    } else if (receiveMessage.id == string_message_id) {
      receiveMessage.getData (s);
      Serial.println (s);
    }
  }
}

