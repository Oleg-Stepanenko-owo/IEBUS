//--------------------------------------------------------------------------------
#include "AVCLan_BT.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

//--------------------------------------------------------------------------------
#define E_LOGG 0

//--------------------------------------------------------------------------------
SoftwareSerial mySerial(4, 3); // RX | TX
char command_buff[15];
int command_i;
bool startCommand;
//--------------------------------------------------------------------------------
void AVCLanBT::begin()
//--------------------------------------------------------------------------------
{
  mySerial.begin(9600);
  mySerial.println("BlueTooth is ready");
  mySerial.println("@log+@ - start logging");
  mySerial.println("@log-@ - end logging");
  mySerial.println("@save@ - save logging in EEPROM");
  logging = false;
  command_i = 0;
  startCommand = false;

  EERPOM_read_config();
}

//--------------------------------------------------------------------------------
void AVCLanBT::println( char* val )
//--------------------------------------------------------------------------------
{
  mySerial.println( val );
}

//--------------------------------------------------------------------------------
void AVCLanBT::print( const char* val )
//--------------------------------------------------------------------------------
{
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
  if ( (command == '@')  && !startCommand ) {
    startCommand = true;
    command_i = 0;
  } else if ( startCommand && (command != '@') ) {

    command_buff[command_i++] = command;
  }	else if ( startCommand && (command == '@') ) {
    command_buff[command_i] = '\0';
    startCommand = false;

    if ( 0 == strcmp( command_buff, "log+" )  ) {
      logging = true;
      mySerial.println(">>Logging ON<<");
      //      Serial.println(">>Logging ON<<");
    } else if ( 0 == strcmp( command_buff, "log-" )  ) {
      mySerial.println(">>Logging OFF<<");
      //      Serial.println(">>Logging OFF<<");
      logging = false;
    } else if ( 0 == strcmp( command_buff, "save" ) ) {
      EEPROM.write( E_LOGG, (int)logging );
    }

    println(command_buff);
    //    Serial.println(command_buff);
  }
}

//--------------------------------------------------------------------------------
void AVCLanBT::println(void)
//--------------------------------------------------------------------------------
{
  print("\r\n");
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
void AVCLanBT::EERPOM_read_config()
//--------------------------------------------------------------------------------
{
  if (EEPROM.read(E_LOGG) == 1 ) logging = true;

  logging ? mySerial.println(">>Logging ON<<") : mySerial.println(">>Logging OFF<<");
}

AVCLanBT avclanBT;


