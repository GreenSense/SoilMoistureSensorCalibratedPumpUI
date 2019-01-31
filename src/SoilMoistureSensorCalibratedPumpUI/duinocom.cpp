#include "Arduino.h"
#include "duinocom.h"

bool verboseCom = false;

bool isMsgReady = false;
int msgPosition = 0;
char msgBuffer[MAX_MSG_LENGTH];
int msgLength = 0;

bool SERIAL_ENABLED = 1;
bool SERIAL1_ENABLED = 0;
bool SERIAL2_ENABLED = 0;
bool SERIAL3_ENABLED = 0;

void setupDuinocom(int baudRate, bool serialEnabled, bool serial1Enabled, bool serial2Enabled, bool serial3Enabled)
{
  SERIAL_ENABLED=serialEnabled;
  SERIAL1_ENABLED=serial1Enabled;
  SERIAL2_ENABLED=serial2Enabled;
  SERIAL3_ENABLED=serial3Enabled;
    
  if (SERIAL_ENABLED)
  {
    Serial.begin(baudRate);

    #ifdef debug
   
      Serial.begin(baudRate);
      while(!Serial);

    #endif
  }
  
  if (SERIAL1_ENABLED)
  {
    Serial1.begin(baudRate);

    #ifdef debug
   
      Serial1.begin(baudRate);
      while(!Serial1);

    #endif
  }
  
  if (SERIAL2_ENABLED)
  {
    Serial2.begin(baudRate);

    #ifdef debug
   
      Serial2.begin(baudRate);
      while(!Serial2);

    #endif
  }
  
  if (SERIAL3_ENABLED)
  {
    Serial3.begin(baudRate);

    #ifdef debug
   
      Serial3.begin(baudRate);
      while(!Serial3);

    #endif
  }
}

// Check whether a message is available and add it to the 'msgBuffer' buffer
bool checkMsgReady()
{
  /*if (SERIAL_ENABLED)
  {
    if (Serial.available() > 0) {
      if (verboseCom)
      {
        Serial.println("Reading serial...");
      }
      byte b = Serial.read();
      
      return processMsgByte(b);
    }
  }*/
   
  if (SERIAL1_ENABLED)
  {   
    if (Serial1.available() > 0) {
      if (verboseCom)
      {
        Serial.println("Reading serial1...");
      }
      byte b = Serial1.read();
      
      return processMsgByte(b);
    }
  }
   
  if (SERIAL2_ENABLED)
  { 
    if (Serial2.available() > 0) {
      if (verboseCom)
      {
        Serial.println("Reading serial2...");
      }
      byte b = Serial2.read();
      
      return processMsgByte(b);
    }
  }
   
  if (SERIAL3_ENABLED)
  { 
    if (Serial3.available() > 0) {
      if (verboseCom)
      {
        Serial.println("Reading serial3...");
      }
      byte b = Serial3.read();
      
      return processMsgByte(b);
    }
  }
}

bool processMsgByte(byte b)
{
    // The end of a message
    if ((b == ';'
      || b == '\n'
      || b == '\r')
      && msgPosition > 0
      )
    {
      if (verboseCom)
      {
        Serial.print("In:");
        if (b == '\n'
          || b == '\r')
          Serial.println("[newline]");
        else
          Serial.println(char(b));
      }

      msgBuffer[msgPosition] = '\0';
      isMsgReady = true;
      msgPosition = 0;

      if (verboseCom)
      {
        Serial.println("Message ready");

        Serial.print("Length:");
        Serial.println(msgLength);
      }
    }
    else if (byte(b) == '\n' // New line
      || byte(b) == '\r') // Carriage return
    {
      if (verboseCom)
        Serial.println("[newline]");
    }
    else // Message bytes
    {
      if (msgPosition == 0)
        clearMsg(msgBuffer);

      msgBuffer[msgPosition] = b;
      msgLength = msgPosition+1;
      msgPosition++;
      isMsgReady = false;

      if (verboseCom)
      {
        Serial.print("In:");
        Serial.println(char(b));
      }
    }

//    delay(1);
 //   delay(15);
    delay(15);
  //}

  return isMsgReady;
}

// Get the message from the 'msgBuffer' buffer
char* getMsg()
{
  // Reset the isMsgReady flag until a new message is received
  isMsgReady = false;

  if (verboseCom)
   printMsg(msgBuffer);

  return msgBuffer;
}

int getMsgLength()
{
  return msgLength;
}

void printMsg(char msg[MAX_MSG_LENGTH])
{
  if (msgLength > 0)
  {
    Serial.print("Message:");
    for (int i = 0; i < MAX_MSG_LENGTH; i++)
    {
      if (msg[i] != '\0')
        Serial.print(char(msg[i]));
    }
    Serial.println();
  }
}

void clearMsg(char msgBuffer[MAX_MSG_LENGTH])
{
  for (int i = 0; i < MAX_MSG_LENGTH; i++)
  {
    msgBuffer[i] = '\0';
  }
}

char getCmdChar(char msg[MAX_MSG_LENGTH], int characterPosition)
{
  return msg[characterPosition];
}

int readInt(char msg[MAX_MSG_LENGTH], int startPosition, int digitCount)
{
  char buffer[digitCount];

  if (verboseCom)
    Serial.println("Reading int");

  for (int i = 0; i < digitCount; i++)
  {
    buffer[i] = msg[startPosition+i];

    if (verboseCom)
      Serial.println(buffer[i]);
  }

  int number = atoi(buffer);

  return number;
}
