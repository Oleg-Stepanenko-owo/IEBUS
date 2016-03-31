/*
*/
#ifndef AVCLanBT_h
#define AVCLanBT_h

#include <avr/pgmspace.h>
#include "Arduino.h"

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

class AVCLanBT
{
  public:
    void begin ();                    // initialisation, obligatory method

    void println( char*);
    void print(const char*);

    char read();

    bool isLogging();
    int available();
    void checkCommand(char command );

  private:
    bool logging;
};

extern AVCLanBT avclanBT;

#endif


