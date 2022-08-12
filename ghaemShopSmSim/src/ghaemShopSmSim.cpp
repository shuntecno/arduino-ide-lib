/*
 * Be name KHODA (In the Name of ALLAH)
 * 
  ghaemShop.ir GSM SMS SIM Library 
  ghaemShopSmSim.V.3.1.0 
  @1396-02-31
  by Akbar Ghaedi from ghaemShop.ir
  
  http://ghaemShop.ir
  
  http://Ghaem24.ir
  http://MikroTek.ir
  http://Micro24.ir
  http://GhaemComputer.ir
  @1394-09-24..@1396-xx-xx
  
  Salavat Hadye be Agha Emam Zaman(GHAEM AALE MOHAMMAD)
  
  you can download last version from:
    http://www.ghemShop.ir/opensource/arduino/ghaemShopSmSim
    https://github.com/ghaemshop/ghaemShopSmSim
    

  For information on installing libraries, see: http://www.arduino.cc/en/Guide/Libraries

*/

#include "ghaemShopSmSim.h"

///// Constructor /////
ghaemShopSmSim::ghaemShopSmSim(int RX, int TX, int SerialBaudRate) {
  GSMSerial = new SoftwareSerial(RX, TX);
  GSMSerial->begin(SerialBaudRate);
}

String Version() {
	return ghaemShopSmSim_VERSION;
}

int ghaemShopSmSim::begin(String PIN) {
	return Begin(PIN);
}
int ghaemShopSmSim::Begin(String PIN) {
  int r = 0;
  GSMSerial->println("AT");
  r = ConfirmAtCommand(COMMAND_ID_AT, "OK", COMMAND_TIMEOUT);
  if (r  > 0) return r;

  GSMSerial->println("ATE1");    // 0:Echo Off, 1:ON
  delay(RESPONSE_DELAY);
  
  if (PIN.length() > 0) {
    GSMSerial->println("AT+CPIN=" + PIN);
    r = ConfirmAtCommand(COMMAND_ID_AT_CPIN, "READY", COMMAND_TIMEOUT);
	  if (r  > 0) return r;
  }
  
  GSMSerial->println("AT+CREG?");
  r = ConfirmAtCommand(COMMAND_ID_AT_CREG, "OK", COMMAND_TIMEOUT);
  if (r  > 0) return r;

  GSMSerial->println("AT+CSCS=\"IRA\"");		// set char set to IRA
  delay(RESPONSE_DELAY);
  
  GSMSerial->println("AT+CMGF=1"); // text mode
  delay(RESPONSE_DELAY);

  GSMSerial->println("AT+CNMI=2,2,0,0,0");
  delay(RESPONSE_DELAY);
  
  GSMSerial->flush();
  BufferIndex = AT_RX_BUFFER_SIZE;
  SetAtRxBufferNull();
}

void ghaemShopSmSim::GSMRecieveEvent() {
  if (GSMSerial->available()) {
    char InChar = GSMSerial->read();
    if (BufferIndex < AT_RX_BUFFER_SIZE) {
      if (InChar == '\r' || InChar == '\n'){
        if (BufferIndex > -1) {
          BufferIndex++;
          AtRxBuffer[BufferIndex] = '\0';
          AtRxBufferStatus = AT_RX_BUFFER_STATUS_COMPLETE;
          int r = GSMDoCommand();
        }
      }
      else {
          BufferIndex++;
          AtRxBuffer[BufferIndex] = InChar;
      }
    }
    else {
      SetAtRxBufferNull();
    }
  }
}

byte ghaemShopSmSim::Status() {
  return AtRxBufferStatus;
}

int ghaemShopSmSim::GSMDoCommand(){
  int Result = NONE;
  int h = 0;
  if (StringStartWith(AtRxBuffer, "OK")) {
    //Serial.print(AtRxBuffer); Serial.println(" -> Find [OK]");
  }
  else if (StringStartWith(AtRxBuffer, "+CMT")) {  // Pars Read SMS Detail & content [+CMT: "]
    //Sample: [+989xxxxxxxxx\",\"\",\"17/05/14,19:43:41+18\"\r\nSMS Content\r\n]
    byte SubscriberNumberLen = 11;
    if (AtRxBuffer[7] == '+') SubscriberNumberLen = 13;
    memmove (LastSubscriberNumber, AtRxBuffer + 7, SubscriberNumberLen);
    LastSubscriberNumber[SubscriberNumberLen] = '\0';
  }
  else if (StringStartWith(AtRxBuffer, "*")) {      // ghaemShop.ir GSM SMS SIM Commander V.1.0.0
    int CommandPartLen = ParsCommand();
    if (CommandPartLen < 3) {
      Result = ERROR_WRONG_COMMAND_PART;
    }
    else {
      if (strcmp(CommandParameters[1], GetPassword()) != 0) {
        Result = ERROR_WRONG_PASSWORD;
      }
      else {
        char* cmd = CommandParameters[0];
        StringToUpper(cmd);

        if (strcmp(cmd, "PASS=") == 0) Result = SetPassword();
        //else if (strcmp(cmd, "PASS?") == 0) Result = GetPassword();
        else if (strcmp(cmd, "RLY=") == 0) Result = SetRelay();
        else if (strcmp(cmd, "RLY?") == 0) Result = GetRelay();
        else if (strcmp(cmd, "VAR=") == 0) Result = SetVariable();
        else if (strcmp(cmd, "VAR?") == 0) Result = GetVariable();
        else Result = ERROR_WRONG_COMMAND;
      }
    }
  }

  SetAtRxBufferNull();
  return Result;
}

void ghaemShopSmSim::SetAtRxBufferNull() {
  memset (AtRxBuffer, NULL, AT_RX_BUFFER_SIZE);
  BufferIndex = -1;   //0
  AtRxBufferStatus = AT_RX_BUFFER_STATUS_NULL;
}

int ghaemShopSmSim::ConfigRelay(struct RelayPinsStruct Relay_Pins[], int PinsCount) {
  RelayPinsCount = PinsCount;
  RelayPins = Relay_Pins;
  
  for (int i = 0; i < PinsCount; i++) {
    pinMode(RelayPins[i].Pin, OUTPUT);
    if (RelayPins[i].InitVal == NULL) RelayPins[i].InitVal = LOW;
    digitalWrite(RelayPins[i].Pin, RelayPins[i].InitVal);   //HIGH, LOW
  }
  RelayPinsConfiged = true;
}

int ghaemShopSmSim::ConfigVariables(Element *Vars, int VarsCount) {
  Variables = Vars;
  VariablesCount = VarsCount;
  VariablesConfiged = true;
}

char* ghaemShopSmSim::GetPassword() {
  char *Pass = new char[PASSWORD_LEN];
  ReadFromEeprom(PASSWORD_ADDRESS_ON_EEPROM, Pass);
  return Pass;
}

int ghaemShopSmSim::SetNewPassword(char* NewPassword) {
  if (strlen(NewPassword) != (PASSWORD_LEN - 1)) return ERROR_EMPTY_NEW_PASSWORD;
  int r = SaveToEeprom(PASSWORD_ADDRESS_ON_EEPROM, NewPassword);
  
  return r;
}

int ghaemShopSmSim::SetPassword() {
  /*
  * Parameters[0] = Command = "PASS="
  * Parameters[1] = Current Password
  * Parameters[2] = New Password
  * Parameters[3] = Confirm (No = 0, Yes > 0 : Delay(ms)
  */
  int Confirm = atoi(CommandParameters[3]);

  if (strlen(CommandParameters[2]) != (PASSWORD_LEN - 1)) return ERROR_EMPTY_NEW_PASSWORD;
  int r = SaveToEeprom(PASSWORD_ADDRESS_ON_EEPROM, CommandParameters[2]);
  String ResultMsg = "*PASS*SET SUCCESSFUL#";
  if (r != SUCCESSFUL) ResultMsg = "*PASS*SET FAILED#";
  
  DeleteAllSmsInSim();
  if (Confirm > 0) ReplyResult(ResultMsg, Confirm);
  return SUCCESSFUL;
}

int ghaemShopSmSim::SetRelay() {
  /*
  * Parameters[0] = Command = "RLY="
  * Parameters[1] = Current Password
  * Parameters[2] = Relay Number or Name
  * Parameters[3] = New Value (0:OFF, 1:ON)
  * Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms)
  */

  //////////////////
  int RelayNumber = -1;
  if (IsDigit(CommandParameters[2])) 
    RelayNumber = atoi(CommandParameters[2]);
  else {
    for (int i = 0; i < RelayPinsCount; i++) {
      if (strcmp(CommandParameters[2], RelayPins[i].Name) == 0) {
        RelayNumber = i;
        break;
      }
    }
  }
  int NewValue = NULL;
  bool P3IsDigit = IsDigit(CommandParameters[3]);
  if (P3IsDigit) 
    NewValue = atoi(CommandParameters[3]);
  else {
    StringToUpper(CommandParameters[3]);
    if (strcmp(CommandParameters[3], "ON") == 0)
      NewValue = HIGH;
    else if (strcmp(CommandParameters[3], "OFF") == 0)
      NewValue = LOW;
  }
  if (NewValue != LOW && NewValue != HIGH) return ERROR_RELAY_SET_PIN_BAD_VALUE;
  if (RelayPins[RelayNumber].NOT) NewValue = (!NewValue);
  int Confirm = atoi(CommandParameters[4]);
  if (!RelayPinsConfiged) return ERROR_RELAY_PINS_NOT_CONFIG;
  if (RelayNumber > RelayPinsCount) return ERROR_RELAY_PINS_BAD_CONFIG;
  digitalWrite(RelayPins[RelayNumber].Pin, NewValue);
  
  DeleteAllSmsInSim();
  if (Confirm > 0) {
    NewValue = digitalRead(RelayPins[RelayNumber].Pin);
    if (RelayPins[RelayNumber].NOT) NewValue = (!NewValue);
    String ReplyValue = String(NewValue);
    if (!P3IsDigit) ReplyValue = (NewValue == HIGH) ? "ON" : "OFF";
    String ResultMsg = "*RLY*" + String(CommandParameters[2]) + "*" + String(ReplyValue) + "#";
    ReplyResult(ResultMsg, Confirm);
  }
  return SUCCESSFUL;
}

int ghaemShopSmSim::GetRelay() {
  /*
  * Parameters[0] = Command = "RLY?"
  * Parameters[1] = Current Password
  * Parameters[2] = Relay Number
  * Out Message = Current Value (0:OFF, 1:ON)
  */

  //////////////////
  int RelayNumber = -1;
  if (IsDigit(CommandParameters[2])) 
    RelayNumber = atoi(CommandParameters[2]);
  else {
    for (int i = 0; i < RelayPinsCount; i++) {
      if (strcmp(CommandParameters[2], RelayPins[i].Name) == 0) {
        RelayNumber = i;
        break;
      }
    }
  }
  if (!RelayPinsConfiged) return ERROR_RELAY_PINS_NOT_CONFIG;
  if (RelayNumber > RelayPinsCount) return ERROR_RELAY_PINS_BAD_CONFIG;
  int Value = digitalRead(RelayPins[RelayNumber].Pin);
  if (RelayPins[RelayNumber].NOT) Value = (!Value);
  String ReplyValue = String(Value);
  if (!ReplyRelayNameAsNumber) ReplyValue = (Value == HIGH) ? "ON" : "OFF";
  String ResultMsg = "*RLY*" + String(CommandParameters[2]) + "*" + String(ReplyValue) + "#";
  ReplyResult(ResultMsg, 0);
  return SUCCESSFUL;
}

int ghaemShopSmSim::SetVariable() {
  /*
  * Parameters[0] = Command = "VAR="
  * Parameters[1] = Current Password
  * Parameters[2] = Variable Name OR Number [0..n]
  * Parameters[3] = New Value (depend on type)
  * Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms)
  */

  int VarNumber = -1;
  if (IsDigit(CommandParameters[2])) 
    VarNumber = atoi(CommandParameters[2]);
  else {
    for (int i = 0; i < VariablesCount; i++) {
      //Serial.println("Variables[" + String(i) + "].Name = [" + Variables[i].Name + "]");
      if (strcmp(CommandParameters[2], Variables[i].Name) == 0) {
        VarNumber = i;
        break;
      }
    }
  }
  
  if (VarNumber == -1)  return ERROR_VARIABLES_BAD_CONFIG;
  if (!VariablesConfiged) return ERROR_VARIABLES_NOT_CONFIG;
  if (VarNumber > VariablesCount) return ERROR_VARIABLES_BAD_CONFIG;

  String NewValue = "";
  switch (Variables[VarNumber].Type) {
    case ET_BYTE:   Variables[VarNumber].b = atoi(CommandParameters[3]); NewValue = String(Variables[VarNumber].b); break;
    case ET_INT:    Variables[VarNumber].i = atoi(CommandParameters[3]); NewValue = String(Variables[VarNumber].i); break;
    case ET_FLOAT:  Variables[VarNumber].f = atof(CommandParameters[3]); NewValue = String(Variables[VarNumber].f, FLOAT_DECIMAL_POINT); break;
    case ET_DOUBLE: Variables[VarNumber].d = atof(CommandParameters[3]); NewValue = String(Variables[VarNumber].d, DOUBLE_DECIMAL_POINT); break;
    case ET_CHAR:   Variables[VarNumber].c = CommandParameters[3][0]; NewValue = String(Variables[VarNumber].c); break;
    case ET_STRING: strcpy(Variables[VarNumber].s, CommandParameters[3]); NewValue = String(Variables[VarNumber].s); break;
  }
  
  DeleteAllSmsInSim();
  int Confirm = atoi(CommandParameters[4]);
  if (Confirm > 0) {
    String ResultMsg = "*VAR*" + String(CommandParameters[2]) + "*" + NewValue + "#";
    ReplyResult(ResultMsg, Confirm);
  }
  return SUCCESSFUL;
}

int ghaemShopSmSim::GetVariable() {
  /*
  * Parameters[0] = Command = "VAR?"
  * Parameters[1] = Current Password
  * Parameters[2] = Variable Name OR Number [0..n]
  */

  int VarNumber = -1;
  //if (P2IsDigit) 
  bool P2IsDigit = IsDigit(CommandParameters[2]);
  if (P2IsDigit) {//IsDigit(Parameters[2])) {
    VarNumber = atoi(CommandParameters[2]);
  }
  else {
    for (int i = 0; i < VariablesCount; i++) {
      if (strcmp(CommandParameters[2], Variables[i].Name) == 0) {
        VarNumber = i;
        break;
      }
    }
  }
  if (VarNumber == -1)  return ERROR_VARIABLES_BAD_CONFIG;
  if (!VariablesConfiged) return ERROR_VARIABLES_NOT_CONFIG;
  if (VarNumber > VariablesCount) return ERROR_VARIABLES_BAD_CONFIG;

  String CurrentValue = "";
  switch (Variables[VarNumber].Type) {
    case ET_BYTE:   CurrentValue = String(Variables[VarNumber].b); break;
    case ET_INT:    CurrentValue = String(Variables[VarNumber].i); break;
    case ET_FLOAT:  CurrentValue = String(Variables[VarNumber].f, FLOAT_DECIMAL_POINT); break;
    case ET_DOUBLE: CurrentValue = String(Variables[VarNumber].d, DOUBLE_DECIMAL_POINT); break;
    case ET_CHAR:   CurrentValue = String(Variables[VarNumber].c); break;
    case ET_STRING: CurrentValue = String(Variables[VarNumber].s); break;
  }
  String ResultMsg = "*VAR*" + String(CommandParameters[2]) + "*" + CurrentValue + "#";
  ReplyResult(ResultMsg, 0);
  return SUCCESSFUL;
}

int ghaemShopSmSim::ReplyResult(String Message, int DelayTime) {
  DeleteAllSmsInSim();
  
  if (DelayTime <= 0) DelayTime = RESPONSE_DELAY;
  if (DelayTime >= COMMAND_TIMEOUT) DelayTime = COMMAND_TIMEOUT;
  delay(DelayTime);
  SendSMS(LastSubscriberNumber, Message);
  
  return SUCCESSFUL;
}

int ghaemShopSmSim::SaveToEeprom(int StartAddress, char* string) {
  int addr = StartAddress;
  for (int i = 0; i <= strlen(string); i++) {
    EEPROM.write(addr, string[i]);
    addr++;
  }
  return SUCCESSFUL;
}

int ghaemShopSmSim::ReadFromEeprom(int StartAddress, char* string) {
  int addr = StartAddress;
  for (int i = 0; i <= strlen(string); i++) {
    string[i] = EEPROM.read(addr);
    addr++;
  }
}

void ghaemShopSmSim::SendSMS(String SMSNumber, String Message) {
  //Send SMS in text mode: AT+CMGS=<number><CR><message><CTRL-Z>
  GSMSerial->println("AT+CMGS=\"" + SMSNumber + "\""); //\r");
  delay(RESPONSE_DELAY);
  GSMSerial->println(Message + "\x1A");   //CTRL_Z = \x1A
}

int ghaemShopSmSim::DeleteSmsFromSim(int SmsID) {
  GSMSerial->print("AT+CMGD=" + SmsID);      // Send SMS Detail & content Read Command
  delay(RESPONSE_DELAY);
}

int ghaemShopSmSim::DeleteAllSmsInSim() {
  GSMSerial->println("AT+CMGDA=\"DEL ALL\"");
  delay(RESPONSE_DELAY);
}

int ghaemShopSmSim::SetSimPin(String PIN) {
  //AT+CPIN=...
  GSMSerial->println("AT+CPIN=" + PIN);
  delay(RESPONSE_DELAY);
}

int ghaemShopSmSim::SetSmsFormatAsText() {
  //AT+CMGF=1
  GSMSerial->println("AT+CMGF=1");
  delay(RESPONSE_DELAY);
}

int ghaemShopSmSim::SetSmsFormatAsPDU() {
  //AT+CMGF=0
  GSMSerial->println("AT+CMGF=0");
  delay(RESPONSE_DELAY);
}

int ghaemShopSmSim::SetCharSet(String CharSet) {
  //AT+CSCS="IRA", "UCS2", ...
  GSMSerial->println("AT+CSCS=\"" + CharSet + "\"");
  delay(RESPONSE_DELAY);
}

void ghaemShopSmSim::AtCommand(const char* Command) {
  GSMSerial->write(Command);
  delay(RESPONSE_DELAY);
  GSMSerial->flush();
}

void ghaemShopSmSim::Write(uint8_t Value) {
  GSMSerial->write(Value);
}

void ghaemShopSmSim::Write(const char* Value) {
  GSMSerial->write(Value);
}

byte ghaemShopSmSim::Println(const char* Value) {
  return GSMSerial->println(Value);
}

byte ghaemShopSmSim::println(const char* Value) {
  return Println(Value);
}

int ghaemShopSmSim::Read() {
  return GSMSerial->read();
}
      
int ghaemShopSmSim::ConfirmAtCommand(int CommandID, char *ExpectedResponse, unsigned long TimeOut) {
  /*
   * RETURN:
   *     0       ExpectedResponse found successfully
   *     1       ERROR string encountered, potential problem
   *     2       Buffer full, ExpectedResponse not found
   *     3       Timeout reached before ExpectedResponse found
   */
  char atRxBuffer[AT_RX_BUFFER_SIZE];
  int index = 0;
  unsigned long tOut = millis();
  while((millis() - tOut) <= TimeOut) {
    if (GSMSerial->available())
    {
      atRxBuffer[index] = GSMSerial->read();
      index++;
      atRxBuffer[index] = '\0';
      if(strstr(atRxBuffer, ExpectedResponse) != NULL) {
        GSMSerial->flush();
        return 0;
      }
      if(strstr(atRxBuffer, "ERROR") != NULL) {
        GSMSerial->flush();
        return 1 + CommandID;
      }
      if(index >= (AT_RX_BUFFER_SIZE - 1)) {
        GSMSerial->flush();
        return 2 + CommandID;
      }
    }
  }
  GSMSerial->flush();
  return 3 + CommandID;
}

bool ghaemShopSmSim::StringStartWith(const char *MainString, const char *SearchString) {
  return memcmp(MainString, SearchString, strlen(SearchString)) == 0 ? true : false;
}

int ghaemShopSmSim::ParsCommand() {
  #define DELIMETER   "*#"
  char * pch;
  byte i = 0;
  
  pch = strtok(AtRxBuffer, DELIMETER);
  while (pch != NULL)
  {
    if (strlen(pch) > 0) strcpy(CommandParameters[i], pch);
    pch = strtok(NULL, DELIMETER);  //DelimeterString);
    i++;
  }
  return i;
}

void ghaemShopSmSim::StringToUpper(char* string) {
  while ( *string != '\0' ) {
    *string = toupper ((unsigned char) *string);
    ++string;
  }
}

bool ghaemShopSmSim::IsDigit(char* string) {
  for (int i = 0; i < strlen(string); i++) {
    if (string[i] != ' ') {
      if (isdigit(string[i])) 
        return true;
      else
        return false;
    }
  }
  return false;
}
///// END ghaemShopSmSim Lib /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

