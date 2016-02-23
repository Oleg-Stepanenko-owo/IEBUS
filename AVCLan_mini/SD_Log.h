#ifndef SD_LOG_h
#define SD_LOG_h

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>

class SDLog {
  public:
    void begin();
    bool bRedyToLog;
    void logs(String dataString);
    void testLog();

    void SerialPrintLn( const char *val );
    void SerialPrint( const char *val );

    void sendByte(uint8_t data);
    void printHex4(uint8_t data);
    void printHex8(uint8_t data);

    void print(const char *pBuf);
    void print(const char pBuf);

    // print string from flash
    void print_p(const char *pBuf);
    void println(const char *pBuf);
    void println(const char pBuf);
    void println(void);

    void _update();
  private:
    bool openFile();
    //    bool canLog;
};

extern SDLog bSDLog;

#endif //#SD_LOG_h



