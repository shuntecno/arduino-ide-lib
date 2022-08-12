/*
# ghaemShopSmSim
arduino SMS Remote Controler over ghaemShopSmSim protocol

 *
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

  * Salavat Hadye be Agha Emam Zaman(GHAEM AALE MOHAMMAD)<br/>

you can download last version from:<br/>
  http://www.ghemShop.ir/opensource/arduino/ghaemShopSmSim
  https://github.com/ghaemshop/ghaemShopSmSim


For information on installing libraries, see: http://www.arduino.cc/en/Guide/Libraries

ghaemShopSmSim protocol:
ghaemShopSmSim v.3.1.0 protocol:
/*******************************************************************************
  
  
  SetPassword()
    
    Command -> *PASS=*Current Password*New Password*Confirm#
    
    /*
    * Parameters[0] = Command = "PASS="
    * Parameters[1] = Current Password
    * Parameters[2] = New Password
    * Parameters[3] = Confirm (No = 0, Yes > 0 : Delay(ms))
    * /



  SetRelay()
  
    Command -> *RLY=*Current Password*Relay Number or Name*New Value*Confirm#
    Sample  -> *RLY=*pass*1*1*3000#
    Sample  -> *RLY=*pass*Motor*ON*3000#
    
    /*
    * Parameters[0] = Command = "RLY="
    * Parameters[1] = Current Password
    * Parameters[2] = Relay Number or Name
    * Parameters[3] = New Value (0:OFF, 1:ON)
    * Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms))
    * /

    
    
  GetRelay()
  
    Command -> *RLY?*Current Password*Relay Number or Name#
    Sample  -> *RLY?*pass*1#
    Sample  -> *RLY?*pass*Motor#

    /*
    * Parameters[0] = Command = "RLY?"
    * Parameters[1] = Current Password
    * Parameters[2] = Relay Number
    * Out Message = Current Value (0:OFF, 1:ON)
    * /

    
    
  SetVariable()
  
    Command -> *VAR=*Current Password*Variable Number or Name*New Value*Confirm#
    Sample  -> *VAR=*pass*1*10*3000#
    Sample  -> *VAR=*pass*MotorSpeed*10*3000#

    /*
    * Parameters[0] = Command = "VAR="
    * Parameters[1] = Current Password
    * Parameters[2] = Variable Name OR Number [0..n]
    * Parameters[3] = New Value (depend on type)
    * Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms))
    * /

    
    
  GetVariable()
  
    Command -> *VAR?*Current Password*Relay Number or Name#
    Sample  -> *VAR?*pass*1#
    Sample  -> *VAR?*pass*Temperture#

    /*
    * Parameters[0] = Command = "VAR?"
    * Parameters[1] = Current Password
    * Parameters[2] = Variable Name OR Number [0..n]
    * /

*******************************************************************************/



#ifndef ghaemShopSmSim_H
  #define ghaemShopSmSim_H
  
  #include <Arduino.h>
  #include <EEPROM.h>
  #include <SoftwareSerial.h>


  //////////////
  #define ghaemShopSmSim_VERSION			      "3.1.0"
  
  #define AT_RX_BUFFER_SIZE         		    100
  #define AT_RX_BUFFER_STATUS_NULL        	1
  #define AT_RX_BUFFER_STATUS_COMPLETE    	2
  #define AT_RX_BUFFER_STATUS_OVERFLAW    	3
  
  #define RESPONSE_DELAY   		              300       // ms
  #define COMMAND_TIMEOUT		                3000		  //ms
  
  #define FLOAT_DECIMAL_POINT               2
  #define DOUBLE_DECIMAL_POINT              2

  // COMMAND_IDs
  #define COMMAND_ID_UNKNOWN		            0000
  #define COMMAND_ID_AT				              1001
  #define COMMAND_ID_AT_CPIN		            1002
  #define COMMAND_ID_AT_CREG		            1003

  
  #define NONE                              0
  #define SUCCESSFUL                        1000
  #define ERROR_WRONG_COMMAND               1100
  #define ERROR_WRONG_COMMAND_PART          1101
  #define ERROR_WRONG_PASSWORD              1102
  #define ERROR_EMPTY_NEW_PASSWORD          1103
  #define ERROR_RELAY_PINS_NOT_CONFIG       1104
  #define ERROR_RELAY_PINS_BAD_CONFIG       1105
  #define ERROR_VARIABLES_NOT_CONFIG        1106
  #define ERROR_VARIABLES_BAD_CONFIG        1107
  #define ERROR_RELAY_SET_PIN_BAD_VALUE     1108
  
  #define PARAMETER_MAX_LEN                 11    // 10 chars + \0
  #define MAX_PARAMETER_COUNT               7
  
  #define SUBSCRIBER_NUMBER_LEN             14    // 13 chars + \0
  #define PASSWORD_LEN                      5     // 4  chars + \0
  #define PASSWORD_ADDRESS_ON_EEPROM        0
  
  ///// Relay Pins /////
  struct RelayPinsStruct {
    char   Name[11];    // 10 chars + \0
    int    Pin;         // Arduino Pin Number
    bool   NOT;         //if Pin Not set true else set false
    int    InitVal;     //HIGH, LOW on start
  };

  ///// Elements-Variables /////
  enum ElementType {
    ET_BYTE,
    ET_INT,
    ET_FLOAT,
    ET_DOUBLE,
    ET_CHAR,
    ET_STRING
  };
  
  struct Element {
    char       Name[11];    // 10 chars + \0
    ElementType Type;
    union {
      byte      b;
      int       i;
      float     f;
      double    d;
      char      c;
      char      s[11];    // 10 chars + \0
    };
  };

  class ghaemShopSmSim {
    
    public:
      bool ReplyRelayNameAsNumber = false;    // defult value is false

      ///// Functions /////
      ghaemShopSmSim(int RX, int TX, int SerialBaudRate);
	    String Version();
      int Begin(String PIN = "");
	    int begin(String PIN = "");
      void GSMRecieveEvent();
      int GSMDoCommand();
      byte Status();
      char* GetPassword();
      int SetNewPassword(char* NewPassword);
      int ConfigRelay(struct RelayPinsStruct Relay_Pins[], int PinsCount);
      int ConfigVariables(Element *Vars, int VariablesCount);
      int SetSimPin(String PIN);
      int SetSmsFormatAsText();
      int SetSmsFormatAsPDU();
      int SetCharSet(String CharSet);
      void AtCommand(const char* Command);
      void Write(uint8_t Value);
      void Write(const char* Value);
      byte Println(const char* Value);
	    byte println(const char* Value);
      int Read();
      int ConfirmAtCommand(int CommandID, char *Response, unsigned long timeOut);
      void SendSMS(String SMSNumber, String Message);
      int DeleteSmsFromSim(int SmsID);
      int DeleteAllSmsInSim();
      
    
    private:
      SoftwareSerial* GSMSerial;
      
      char AtRxBuffer[AT_RX_BUFFER_SIZE];
      int BufferIndex = -1;
      byte AtRxBufferStatus = AT_RX_BUFFER_STATUS_NULL;
      char CommandParameters[MAX_PARAMETER_COUNT][PARAMETER_MAX_LEN];
    
      int VariablesCount;
      struct Element *Variables;
      bool VariablesConfiged = false;
  
      char LastSubscriberNumber[SUBSCRIBER_NUMBER_LEN];

      struct RelayPinsStruct *RelayPins;
      bool RelayPinsConfiged = false;
      int RelayPinsCount;

      ///// Functions /////
      void SetAtRxBufferNull();
      int ParsCommand();
      int SetPassword();
      int SetRelay();
      int GetRelay();
      int SetVariable();
      int GetVariable();
      int ReplyResult(String Message, int DelayTime);
      int SaveToEeprom(int StartAddress, char* string);
      int ReadFromEeprom(int StartAddress, char* string);
      bool StringStartWith(const char *MainString, const char *SearchString);
      int InStrChar(const char *MainString, char SearchChar);
      void StringToUpper(char* string);
      bool IsDigit(char* string);
      int CountCharIttration(const char *MainString, char SearchChar);
  };

#endif

///// END ghaemShopSmSim Lib Header File /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

