#include "SD_Log.h"
#include "config.h"
//--------------------------------------------------------------------------------

const int chipSelect = 10;
static File dataFile;
char fileName[10] = {0};

//--------------------------------------------------------------------------------
void SDLog::begin()
//--------------------------------------------------------------------------------
{
  //  canLog = false;
  bRedyToLog = false;
//  // see if the card is present and can be initialized:
//  if (SD.begin(chipSelect)) {
//    bRedyToLog = true;
//    Serial.println(" READY TO LOG ");
//  }
}

//--------------------------------------------------------------------------------
void SDLog::testLog()
//--------------------------------------------------------------------------------
{
  char filetext[40] = {0};

  File dataTestFile = SD.open( "test.txt", FILE_WRITE );
  if ( bRedyToLog && dataTestFile ) {
    sprintf(filetext, "[%u] IEBUS HONDA V." IEBUS_VERSION, millis() );
    dataTestFile.println(filetext);
    openFile();
  } else {
    bRedyToLog = false;
  }
  dataTestFile.close();
}

//--------------------------------------------------------------------------------
bool SDLog::openFile()
//--------------------------------------------------------------------------------
{
  for ( int a = 1; a < 999; a++)
  {
    sprintf( fileName, "%d.txt", a );
    if ( !SD.exists( fileName ) )
      break;
  }

  dataFile = SD.open( fileName, FILE_WRITE );
  if ( dataFile ) {
    logs( String("IEBUS HONDA V.") + String(IEBUS_VERSION) );
    dataFile.close();
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------
void SDLog::logs(String dataString)
//--------------------------------------------------------------------------------
{
  //SerialPrintLn(dataString.c_str());
  dataFile = SD.open( fileName, FILE_WRITE );
  if ( bRedyToLog && dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
}

//--------------------------------------------------------------------------------
void SDLog::SerialPrintLn( const char *val )
//--------------------------------------------------------------------------------
{
  if ( Serial ) {
    Serial.println( val );
  }
}

//--------------------------------------------------------------------------------
void SDLog::SerialPrint( const char *val )
//--------------------------------------------------------------------------------
{
  if ( Serial ) {
    Serial.print( val );
  }
}
//--------------------------------------------------------------------------------
void SDLog::sendByte(uint8_t data)
//--------------------------------------------------------------------------------
{
  if (bRedyToLog) {
    dataFile.close();
    dataFile = SD.open( fileName, FILE_WRITE );
    dataFile.print( (char)data );
    dataFile.close();

    // SerialPrint(data);
    /*
       BASE (optional): the base sin which to print numbers:
       BIN for binary (base 2),
       DEC for decimal (base 10),
       OCT for octal (base 8),
       HEX for hexadecimal (base 16).
    */
  }
}

//--------------------------------------------------------------------------------
void SDLog::printHex4(uint8_t data)
//--------------------------------------------------------------------------------
{
  uint8_t c = data & 0x0f;
  c += c < 10 ? '0' : 'A' - 10 ;
  sendByte(c);
}

//--------------------------------------------------------------------------------
void SDLog::printHex8(uint8_t data)
//--------------------------------------------------------------------------------
{
  printHex4(data >> 4);
  printHex4(data);
}

//--------------------------------------------------------------------------------
void SDLog::print(const char *pBuf)
//--------------------------------------------------------------------------------
{
  while (*pBuf) {
    sendByte(*pBuf++);
  }
}

//--------------------------------------------------------------------------------
void SDLog::print(const char pBuf)
//--------------------------------------------------------------------------------
{
  sendByte(pBuf);
}

//--------------------------------------------------------------------------------
void SDLog::println(const char *pBuf)
//--------------------------------------------------------------------------------
{
  print(pBuf);
  println();
}

//--------------------------------------------------------------------------------
void SDLog::println(const char pBuf)
//--------------------------------------------------------------------------------
{
  print(pBuf);
  println();
}

//--------------------------------------------------------------------------------
void SDLog::println(void)
//--------------------------------------------------------------------------------
{
  print("\n\r");
}

//void SDLog::_update()
//{
//  if ( bRedyToLog ) {
//    dataFile.close();
//  }
//}

SDLog bSDLog;
