#ifndef duinocom_H_
#define duinocom_H_

const int MAX_MSG_LENGTH = 20;

void setupDuinocom(int baudRate, bool serialEnabled, bool serial1Enabled, bool serial2Enabled, bool serial3Enabled);

bool checkMsgReady();

bool processMsgByte(byte b);

char* getMsg();

int getMsgLength();

void printMsg(char msg[MAX_MSG_LENGTH]);

void clearMsg(char msg[MAX_MSG_LENGTH]);

void identify();

char getCmdChar(char msg[MAX_MSG_LENGTH], int characterPosition);

int readInt(char msg[MAX_MSG_LENGTH], int startPosition, int digitCount);

#endif
