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

    void println( char*);
    void print(const char*);
    void printHex4(uint8_t);
    void printHex8(uint8_t);
    void printDec(uint8_t);
    void println(void);

    void sendByte(uint8_t);
    char read();

    bool isLogging();
    int available();
    void checkCommand(char command );
    void printAction( AvcActionID ActionID );

  private:
    void EERPOM_read_config();
    bool logging;
};

extern AVCLanBT avclanBT;

#endif


