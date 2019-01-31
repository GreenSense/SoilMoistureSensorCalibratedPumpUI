#include <LiquidCrystal.h>
#include <duinocom.h>

bool isDebug = 1;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define MENU_OPTION_MOISTURE              0
#define MENU_OPTION_PUMP                  1
#define MENU_OPTION_THRESHOLD             2
#define MENU_OPTION_READINGINTERVAL       3
#define MENU_OPTION_PUMPBURSTONDURATION   4
#define MENU_OPTION_PUMPBURSTOFFDURATION  5

#define MAX_MENU_OPTIONS      6

int currentMenuState = MENU_OPTION_MOISTURE;

long lastButtonPress = 0;
long buttonInterval = 200;

bool didChange = false;

int soilMoistureValue = 50;
int soilMoistureThreshold = 30;

int pumpBurstOnDuration = 3;
int pumpBurstOffDuration = 3;

int readingInterval = 3;

int pumpStatus = 0;

void setup()
{
  // Begin Serial and Serial3 at 9600 Baud
 setupDuinocom(9600, 1, 0, 0, 1);
  
 lcd.begin(16, 2);              // start the library
 
 lcd.write("Loading...");
}
 
void loop()
{ 
  handleButtons();
  
  handleMsg();

  drawMenu();
}

void handleButtons()
{
  // Buttons are temporarily disabled to avoid duplicate presses
  bool buttonsAreAvailable = lastButtonPress + buttonInterval < millis();
  
  if (buttonsAreAvailable)
  {
    lcd_key = read_LCD_buttons();  // read the buttons

    handleButton(lcd_key);
    
    lastButtonPress = millis();
  }
}

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor 
 
 // Serial.println(adc_key_in);
 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 
 // For V1.1 us this threshold
 /*if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT; */

 // For V1.0 comment the other threshold and use the one below:

 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   



 return btnNONE;  // when all others fail, return this...
}

void handleMsg()
{
  // If the soil moisture is being display then it's in "read" mode, otherwise it's in "edit settings" mode
  bool isEditMode = currentMenuState != MENU_OPTION_MOISTURE;
  
  // Stop receiving messages/data when in settings edit mode
  if (!isEditMode)
  {
    if (checkMsgReady())
    {
      char* msg = getMsg();
          
      char letter = msg[0];

      int length = strlen(msg);

      Serial.print("Received message: ");
      Serial.println(msg);

		  if (getColonPosition(msg) > 0)
		  {
			  parseData(msg);
      }
	  }
	}
}

void handleButton(char lcd_key)
{
 switch (lcd_key)
 {
   case btnRIGHT:
   {
      menuRight();
      break;
   }
   case btnLEFT:
   {
     menuLeft();
     break;
   }
   case btnUP:
   {
     menuUp();
     break;
   }
   case btnDOWN:
   {
     menuDown();
     break;
   }
   case btnSELECT:
   {
     menuSelect();
     break;
   }
   case btnNONE:
   {
     break;
   }
 }
}

void drawMenu()
{
  if (didChange)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    // lcd.print(currentMenuState);
     
     switch (currentMenuState)
     {
        case MENU_OPTION_MOISTURE:
          lcd.print("Moisture: ");          
          lcd.print(soilMoistureValue);
          lcd.print("%");
          break;
        case MENU_OPTION_PUMP:
          lcd.print("Pump: ");     
          lcd.print(getPumpStatusText(pumpStatus));
          break;
        case MENU_OPTION_THRESHOLD:
          lcd.print("Threshold: ");     
          lcd.print(soilMoistureThreshold);
          lcd.print("%");        
          break;
        case MENU_OPTION_READINGINTERVAL:
          lcd.print("Readings: ");     
          lcd.print(readingInterval);
          lcd.print("s");        
          break;
        case MENU_OPTION_PUMPBURSTONDURATION:
          lcd.print("Pump On: ");          
          lcd.print(pumpBurstOnDuration);
          lcd.print("s");
          break;
        case MENU_OPTION_PUMPBURSTOFFDURATION:
          lcd.print("Pump Off: ");          
          lcd.print(pumpBurstOffDuration);
          lcd.print("s");
          break;
     }
     
     // Reset the "did change" flag until something new changes
     didChange = false;
  }
}

void menuLeft()
{
  
  if (currentMenuState <= 0)
    currentMenuState = MAX_MENU_OPTIONS-1;
  else
    currentMenuState--;  
    
  Serial.println("Menu left");
  Serial.print("Menu state");
  Serial.println(currentMenuState);
  
  didChange = true;
}

void menuRight()
{
  if (currentMenuState >= MAX_MENU_OPTIONS-1)
    currentMenuState = 0;
  else
    currentMenuState++;  
    
  Serial.println("Menu right");
  Serial.print("Menu state");
  Serial.println(currentMenuState);
  
  didChange = true;
}

void menuDown()
{
  switch (currentMenuState)
  {
    case MENU_OPTION_THRESHOLD:
      decreaseThreshold();
      break;
    case MENU_OPTION_PUMP:
      pumpLeftOption();
      break;
    case MENU_OPTION_READINGINTERVAL:
      decreaseReadingInterval();
      break;
    case MENU_OPTION_PUMPBURSTONDURATION:
      decreasePumpBurstOnDuration();
      break;
    case MENU_OPTION_PUMPBURSTOFFDURATION:
      decreasePumpBurstOffDuration();
      break;
  }
}

void menuUp()
{
  switch (currentMenuState)
  {
    case MENU_OPTION_THRESHOLD:
      increaseThreshold();
      break;
    case MENU_OPTION_PUMP:
      pumpUpOption();
      break;
    case MENU_OPTION_READINGINTERVAL:
      increaseReadingInterval();
      break;
    case MENU_OPTION_PUMPBURSTONDURATION:
      increasePumpBurstOnDuration();
      break;
    case MENU_OPTION_PUMPBURSTOFFDURATION:
      increasePumpBurstOffDuration();
      break;
  }
}

void menuSelect()
{
  switch (currentMenuState)
  {
    case MENU_OPTION_THRESHOLD:
      sendThresholdCommand();
      break;
    case MENU_OPTION_PUMP:
      sendPumpStatusCommand();
      break;
    case MENU_OPTION_READINGINTERVAL:
      sendReadingIntervalCommand();
      break;
    case MENU_OPTION_PUMPBURSTONDURATION:
      sendPumpBurstOnDurationCommand();
      break;
    case MENU_OPTION_PUMPBURSTOFFDURATION:
      sendPumpBurstOffDurationCommand();
      break;
  }
}

void sendThresholdCommand()
{
  
  Serial.println("Sending threshold command");

  String cmd = "T";
  cmd = cmd + soilMoistureThreshold;
  
  Serial.println(cmd);
  
  Serial3.println(cmd);
  
  lcd.setCursor(0, 1);
  lcd.write("              ->");
  
  clearSerialBuffer();
}

void decreaseThreshold()
{
  soilMoistureThreshold--;
  
  didChange = true;
}

void increaseThreshold()
{
  soilMoistureThreshold++;
  
  didChange = true;
}

void sendPumpBurstOnDurationCommand()
{
  Serial.println("Sending pump burst on time command");

  String cmd = "B";
  cmd = cmd + pumpBurstOnDuration;
  
  Serial.println(cmd);
  
  Serial3.println(cmd);
  
  lcd.setCursor(0, 1);
  lcd.write("              ->");
}

void decreasePumpBurstOnDuration()
{
  pumpBurstOnDuration--;
  
  didChange = true;
}

void increasePumpBurstOnDuration()
{
  pumpBurstOnDuration++;
  
  didChange = true;
}

void sendPumpBurstOffDurationCommand()
{
  
  Serial.println("Sending pump burst off time command");

  String cmd = "O";
  cmd = cmd + pumpBurstOffDuration;
  
  Serial.println(cmd);
  
  Serial3.println(cmd);
  
  lcd.setCursor(0, 1);
  lcd.write("              ->");
}

void decreasePumpBurstOffDuration()
{
  pumpBurstOffDuration--;
  
  didChange = true;
}

void increasePumpBurstOffDuration()
{
  pumpBurstOffDuration++;
  
  didChange = true;
}

void sendReadingIntervalCommand()
{
  
  Serial.println("Sending reading interval command");

  String cmd = "V";
  cmd = cmd + readingInterval;
  
  Serial.println(cmd);
  
  Serial3.println(cmd);
  
  lcd.setCursor(0, 1);
  lcd.write("              ->");
}

void decreaseReadingInterval()
{
  readingInterval--;
  
  didChange = true;
}

void increaseReadingInterval()
{
  readingInterval++;
  
  didChange = true;
}

void sendPumpStatusCommand()
{
  
  Serial.println("Sending pump status command");

  String cmd = "P";
  cmd = cmd + pumpStatus;
  
  Serial.println(cmd);
  
  Serial3.println(cmd);
  
  lcd.setCursor(0, 1);
  lcd.write("              ->");
}

void pumpLeftOption()
{
  if (pumpStatus <= 0)
    pumpStatus = 2;
  else
    pumpStatus--;
  
  Serial.print("Pump status: ");
  Serial.println(pumpStatus);
  
  didChange = true;
}

void pumpUpOption()
{
  if (pumpStatus >= 2)
    pumpStatus = 0;
  else
    pumpStatus++;
    
  Serial.print("Pump status: ");
  Serial.println(pumpStatus);
  
  didChange = true;
}

void clearSerialBuffer()
{
  Serial.println("Clearing serial buffer");
  
  while(Serial3.available() > 0) {
    char t = Serial3.read();
  }
}

void parseData(char* data)
{
  if (isDebug)
  {
    Serial.println("Parsing data...");
    Serial.println(data);
	}

  int colonPosition = getColonPosition(data);


  int keyLength = colonPosition;

  if (isDebug)
  {
    Serial.print("Key length: ");
    Serial.println(keyLength);
  }

  char key[10];
  
  // Figure out a cleaner way to do this
  for (int i = 0; i < 10; i++)
  {
    if (i < keyLength)
      key[i] = data[i];
    else
      key[i] = '\0';
  }


  if (isDebug)
  {
    Serial.print("Key: ");
    Serial.println(key);
  }
  
	int value = getValue(data);

  if (isDebug)
  {
    Serial.print("Value: ");
    Serial.println(value);
  }

	setData(key, value);

  if (isDebug)
  {
    Serial.println();
    Serial.println();
	    
	}
}

void setData(char* key, int value)
{
	Serial.println("Setting data...");

  if (isDebug)
  {
    Serial.print("Key: ");
    Serial.println(key);
    
    Serial.print("Value: ");
    Serial.println(value);
  }


	if (strcmp(key, "C") == 0)
	{
			Serial.print("Soil moisture value (incoming):");
			Serial.println(value);
 
      if (soilMoistureValue != value)
      {
        soilMoistureValue = value;
      
        didChange = true;
      }
	}
	if (strcmp(key, "T") == 0)
	{
			Serial.print("Soil moisture threshold (incoming):");
			Serial.println(value);
 
      if (soilMoistureThreshold != value)
      {
        soilMoistureThreshold = value;
      
        didChange = true;
      }
	}
	if (strcmp(key, "V") == 0)
	{
			Serial.print("Reading interval (incoming):");
			Serial.println(value);
 
      if (readingInterval != value)
      {
        readingInterval = value;
      
        didChange = true;
      }
	}
	if (strcmp(key, "B") == 0)
	{
			Serial.print("Pump burst on duration (incoming):");
			Serial.println(value);
 
      if (pumpBurstOnDuration != value)
      {
        pumpBurstOnDuration = value;
      
        didChange = true;
      }
	}
	if (strcmp(key, "O") == 0)
	{
			Serial.print("Pump burst off duration (incoming):");
			Serial.println(value);
 
      if (pumpBurstOffDuration != value)
      {
        pumpBurstOffDuration = value;
      
        didChange = true;
      }
	}
	if (strcmp(key, "P") == 0)
	{
			Serial.print("Pump status (incoming):");
			Serial.println(value);
 
      if (pumpStatus != value)
      {
        pumpStatus = value;
      
        didChange = true;
      }
	}
}

char* getKey(char* msg)
{

  if (isDebug)
  {
   Serial.println("Getting key");
   Serial.println(msg);
  }

  int colonPosition = getColonPosition(msg);

  if (isDebug)
  {
    Serial.print("  Colon position: ");
    Serial.println(colonPosition);
  }

  int keyLength = colonPosition;

  if (isDebug)
  {
    Serial.print("  Key length: ");
    Serial.println(keyLength);
  }

  char key[5];
  strncpy(key, msg, keyLength);
  
  if (isDebug)
  {
    Serial.print("  Key: ");
    Serial.println(key);

    Serial.println("Finished getting key");
    Serial.println("");
  }

  return key;
}


int getValue(char* msg)
{

  if (isDebug)
  {
    Serial.println("Getting value");
  }

  int colonPosition = getColonPosition(msg);

  if (isDebug)
  {
    Serial.print("  Colon position: ");
    Serial.println(colonPosition);
  }

  int valueLength = strlen(msg)-colonPosition-1;

  if (isDebug)
  {
    Serial.print("  Value length: ");
   Serial.println(valueLength);
  }

  int valueStartPosition = colonPosition+1;

  if (isDebug)
  {
    Serial.print("  Value start position: ");
    Serial.println(valueStartPosition);
  }

  int value = readInt(msg, valueStartPosition, valueLength);
  
  if (isDebug)
  {
    Serial.print("  Value: ");
    Serial.println(value);

    Serial.println("Finished getting value");
    Serial.println("");
  }

  return value;
}

int getColonPosition(char* msg)
{

  if (isDebug)
  {
    Serial.print("Getting colon position from: ");
    Serial.println(msg);
  }

  int colonPosition = 0;

  unsigned int numElements = strlen(msg);
  unsigned int i;
  for(i = 0; i < numElements; ++i) {
      if (msg[i] == ':') {
          colonPosition = i;
          break;
      }
  }

  return colonPosition;
}

char* getPumpStatusText(int pumpStatus)
{
  switch(pumpStatus)
  {
    case 0:
      return "Off";
      break;
    case 1:
      return "On";
      break;
    case 2:
      return "Auto";
      break;
  }
}

