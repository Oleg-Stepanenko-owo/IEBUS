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

  private:
};

extern AVCLanBT avclanBT;

#endif


