//--------------------------------------------------------------------------------
#include "AVCLan_BT.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
//--------------------------------------------------------------------------------
SoftwareSerial mySerial(4, 3); // RX | TX
char command_buff[15];
int command_i;
//--------------------------------------------------------------------------------
void AVCLanBT::begin()
//--------------------------------------------------------------------------------
{
  mySerial.begin(9600);
  mySerial.println("BlueTooth is ready");
  mySerial.println("log_on@ - start logging");
  mySerial.println("log_off@ - end logging");
  mySerial.println("save@ - save logging in EEPROM");
  logging = false;
  command_i = 0;
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
  if ( command == '@' ) //end of command;
  {
    command_buff[command_i + 1] = '\0';
    command_i = 0;

    if ( 0 == strcmp( command_buff, "log_on" )  ) logging = true;
    else if ( 0 == strcmp( command_buff, "log_off" )  ) logging = false;
    println(command_buff);
    Serial.print("logging - "); Serial.println(logging);
  }
  else
  {
    command_buff[command_i++] = command;
  }
}

AVCLanBT avclanBT;


