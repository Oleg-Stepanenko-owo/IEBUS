//--------------------------------------------------------------------------------
#include "AVCLan_BT.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
//--------------------------------------------------------------------------------
// *1 - Rear camera ON
// *2 - Rear camera Off
// *3 - Display off
// *4 - Button donw
// *5 - >>A:BUTTON UP<<"
// *6 ">>A:B_DISP_OFF<<"
// *7 ">>A:B_DISP_FULL_DOWN<<"
// *8 ">>A:B_DISP_FULL_UP<<"
// *9 ">>A:B_DISP_HILF<<"
// *10  ">>A:TEL<<"
// *11  ">>A:TEL CANCEL<<"
// *12 ">>A:PREP_CAMOFF<<"
// *99 ">>A:UNDEFINED<<");
//--------------------------------------------------------------------------------
#define E_LOGG 0
#define E_DISPLAY 1

//--------------------------------------------------------------------------------
SoftwareSerial mySerial(4, 3); // RX | TX
char command_buff[15];
int command_i;
bool startCommand;
//--------------------------------------------------------------------------------
void AVCLanBT::begin()
//--------------------------------------------------------------------------------
{
  dispalyStatus = 0;
  mySerial.begin(9600);
  mySerial.println("BlueTooth:");
  mySerial.println("@l+@");
  mySerial.println("@l-@");
  mySerial.println("@s@");
  mySerial.println("@d0,1,2@");
  // d0 - default
  // d1 - always HONDA
  // d2 - always GVN
  logging = false;
  command_i = 0;
  startCommand = false;

  EERPOM_read_config();
}

//--------------------------------------------------------------------------------
void AVCLanBT::println( char* val, bool bLogging )
//--------------------------------------------------------------------------------
{
  if ( bLogging || logging )
    mySerial.println( val );
}

//--------------------------------------------------------------------------------
void AVCLanBT::print( const char* val, bool bLogging )
//--------------------------------------------------------------------------------
{
  if ( bLogging || logging )
    mySerial.print( val );
}

//--------------------------------------------------------------------------------
char AVCLanBT::read()
//--------------------------------------------------------------------------------
{
  return mySerial.read();
}

//--------------------------------------------------------------------------------
bool AVCLanBT::isLogging()
//--------------------------------------------------------------------------------
{
  return logging;
}

//--------------------------------------------------------------------------------
int AVCLanBT::available()
//--------------------------------------------------------------------------------
{
  return mySerial.available();
}

//--------------------------------------------------------------------------------
void AVCLanBT::checkCommand( char command )
//--------------------------------------------------------------------------------
{
//  Serial.print("command:"); Serial.println(command);

  if ( (command == '@')  && !startCommand ) {
    startCommand = true;
    command_i = 0;
  } else if ( startCommand && (command != '@') ) {

    command_buff[command_i++] = command;
  }	else if ( startCommand && (command == '@') ) {
    command_buff[command_i] = '\0';
    startCommand = false;

    if ( 0 == strcmp( command_buff, "l+" )  ) {
      logging = true;
      mySerial.println("#ON");
    } else if ( 0 == strcmp( command_buff, "l-" )  ) {
      mySerial.println("#OFF");
      logging = false;
    } else if ( 0 == strcmp( command_buff, "s" ) ) {
      EEPROM.write( E_LOGG, (int)logging );
      EEPROM.write( E_DISPLAY, dispalyStatus );
      mySerial.print("#S:"); mySerial.print(logging); mySerial.print(":"); mySerial.println(dispalyStatus);
    } else if ( 0 == strcmp( command_buff, "d0" ) ) {
      dispalyStatus = 0;
      mySerial.println("#D0");
    } else if ( 0 == strcmp( command_buff, "d1" ) ) {
      dispalyStatus = 1;
      mySerial.println("#D1");
    } else if ( 0 == strcmp( command_buff, "d2" ) ) {
      dispalyStatus = 2;
      mySerial.println("#D2");
    }
    // println(command_buff);
  }
}

//--------------------------------------------------------------------------------
void AVCLanBT::println( bool bLogging )
//--------------------------------------------------------------------------------
{
  if ( bLogging || logging )
    mySerial.print("\r\n");
}

//--------------------------------------------------------------------------------
void AVCLanBT::printHex4(uint8_t data)
//--------------------------------------------------------------------------------
{
  uint8_t c = data & 0x0f;
  c += c < 10 ? '0' : 'A' - 10 ;
  sendByte(c);
}

//--------------------------------------------------------------------------------
void AVCLanBT::printHex8(uint8_t data)
//--------------------------------------------------------------------------------
{
  printHex4(data >> 4);
  printHex4(data);
}

//--------------------------------------------------------------------------------
void AVCLanBT::printDec(uint8_t data)
//--------------------------------------------------------------------------------
{
  uint8_t buf[3];
  uint8_t i = 0;
  if (data == 0) {
    sendByte('0');
    return;
  }

  while (data > 0) {
    buf[i++] = data % 10;
    data /= 10;
  }
  for (; i > 0; i--)
    sendByte((buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
}

//--------------------------------------------------------------------------------
void AVCLanBT::sendByte(uint8_t val)
//--------------------------------------------------------------------------------
{
  mySerial.write( val );
}

//--------------------------------------------------------------------------------
void AVCLanBT::printAction( AvcActionID ActionID )
//--------------------------------------------------------------------------------
{
  switch ( ActionID )
  {
    case ACT_CAM_ON: 		mySerial.println("*1");				break;
    case ACT_CAM_OFF: 		mySerial.println("*2");				break;
    case ACT_DISP_OFF: 		mySerial.println("*3");				break;
    case ACT_BUTTON_DOWN: 	mySerial.println("*4");			break;
    case ACT_BUTTON_UP: 	mySerial.println("*5");			break;
    case ACT_B_DISPOFF:		mySerial.println("*6"); 			break;
    case ACT_B_DISPFULL_DOWN: mySerial.println("*7"); 	break;
    case ACT_B_DISPFULL_UP:	mySerial.println("*8");		break;
    case ACT_B_DISPHULF: 	mySerial.println("*9");			break;
    case ACT_TEL: 			mySerial.println("*10");					break;
    case ACT_TEL_CANCEL: 	mySerial.println("*11");			break;
    case ACT_PREP_CAMOFF:   mySerial.println("*12");			break;
    default: 				mySerial.println("*99");
  }
}

//--------------------------------------------------------------------------------
int AVCLanBT::getDisplayStatus()
//--------------------------------------------------------------------------------
{
  return dispalyStatus;
}

//--------------------------------------------------------------------------------
void AVCLanBT::EERPOM_read_config()
//--------------------------------------------------------------------------------
{
  if ( EEPROM.read(E_LOGG) == 1 ) logging = true;
  dispalyStatus = EEPROM.read(E_DISPLAY);

  logging ? mySerial.println("Log+;") : mySerial.println("Log-;");
  mySerial.print("Disp:"); mySerial.println(dispalyStatus);
}

AVCLanBT avclanBT;


