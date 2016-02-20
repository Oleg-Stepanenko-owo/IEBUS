/*
*/
#ifndef AVCLanHonda_h
#define AVCLanHonda_h

#include <avr/pgmspace.h>
#include "AVCLanDrv.h"
#include "Arduino.h"

//--------------------------------------------------------------------------------
// timer1 overflow every 1 sec
#define TI1_H   (((word)-(F_CPU / 1024)) >> 8)
#define TI1_L   (((word)-(F_CPU / 1024)) & 0xff )

#if defined(__AVR_ATmega8__)
#define ENABLE_TIMER1_INT  (sbi(TIMSK, TOIE1));
#define DISABLE_TIMER1_INT (cbi(TIMSK, TOIE1));
#else
#define ENABLE_TIMER1_INT  (sbi(TIMSK1, TOIE1));
#define DISABLE_TIMER1_INT (cbi(TIMSK1, TOIE1));
#endif

// адреса eeprom
#define E_MASTER1       0
#define E_MASTER2       1
#define E_READONLY      2
#define E_INIT          3


#define AVCLANDEVICE_NAME    " Honda Kb1"
#define AVCLANDEVICE_VERSION "0.0.7"

#define BUTT_WAIT 1000
#define LOCK_TIME 700

//--------------------------------------------------------------------------------
typedef enum {
  ACT_CAM_ON = 1,
  ACT_CAM_OFF,
//  ACT_DISP_HULF,
  ACT_DISP_OFF,
  ACT_BUTTON_PRESS,
  ACT_B_DISPOFF,
  ACT_B_DISPFULL,
  ACT_B_DISPHULF,
  ACT_TEL,
  ACT_TEL_CANCEL
//  ACT_VOL
} AvcActionID;


//--------------------------------------------------------------------------------

class AVCLanHonda
{
  public:
    void begin ();                    // initialisation, obligatory method
    void getActionID();               // get action id by recieved message, obligatory method
    void processAction(AvcActionID);  // process action, obligatory method
    byte sendStatus();                // send CD-changer status to head


    bool getCommute();
    void tryToShowHondaDisp();

    void setWaitTime( const unsigned long mTime );
    inline unsigned long getWaitTime() const;

    void setLockTime( const unsigned long mTime );
    inline unsigned long getLockTime() const;

    void setWait( const bool mWait );
    inline bool isWait() const;

    inline bool isShowRearCam() const;
    inline bool isShowHondaDisp() const;

    void checkWait();
    void checkLock();
    void falseHondaDis();
    int mVol;


  private:
    unsigned long waitTime;
    unsigned long lockTime;

    bool bWait;
    bool bLock;

    bool bShowRearCam;
    bool bShowHondaDisp;
    bool bTimeSwitch;
    bool isHondaDisLast;
};

unsigned long AVCLanHonda::getLockTime() const {
  return lockTime;
}

unsigned long AVCLanHonda::getWaitTime() const {
  return waitTime;
}
bool AVCLanHonda::isWait() const {
  return bWait;
};
bool AVCLanHonda::isShowRearCam() const {
  return bShowRearCam;
};
bool AVCLanHonda::isShowHondaDisp() const {
  return bShowHondaDisp;
}

extern AVCLanHonda avclanHonda;

#endif
