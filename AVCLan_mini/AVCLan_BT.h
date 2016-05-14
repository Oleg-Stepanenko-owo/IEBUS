/*
*/
#ifndef AVCLanBT_h
#define AVCLanBT_h

#include <avr/pgmspace.h>
#include "Arduino.h"
#include "AVCLanHonda.h"

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

class AVCLanBT
{
  public:
    void begin ();                    // initialisation, obligatory method

    void println( char*, bool bLogging = false );
    void print(const char*, bool bLogging = false );
    void printHex4(uint8_t);
    void printHex8(uint8_t);
    void printDec(uint8_t);
    void println( bool bLogging = false );

    void sendByte(uint8_t);
    char read();

    bool isLogging();
    int available();
    void checkCommand(char command );
    void printAction( AvcActionID ActionID );

    int getDisplayStatus();

  private:
    void EERPOM_read_config();
    bool logging;
    int dispalyStatus;
};

extern AVCLanBT avclanBT;

#endif


