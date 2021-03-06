//--------------------------------------------------------------------------------
#include <EEPROM.h>
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
//#include "BuffSerial.h"
#include "SD_Log.h"
#include <avr/pgmspace.h>
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//const AvcInMessageTable  mtSearchHead[] PROGMEM = {
//  { ACT_BUTTON_PRESS,   0x08,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x03, {0x21, 0x01, 0xBE}},         // Button press
//  { ACT_B_DISPOFF,      0x08,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x03, {0x22, 0x71, 0x2F}},
//  { ACT_B_DISPFULL,     0x08,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x03, {0x20, 0x01, 0xBD}},
//  { ACT_B_DISPHULF,     0x08,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x03, {0x22, 0x11, 0xCF}},
//  { ACT_CAM_ON,         0x09,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x04, {0x94, 0x00, 0x00, 0x31}},    // Cam ON
//  { ACT_CAM_OFF,        0x09,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x04, {0x94, 0xFF, 0x00, 0x30}},    // Cam OFF
//  { ACT_DISP_HULF,      0x09,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x04, {0x91, 0x02, 0x00, 0x30}},    // display hulf color
//  { ACT_DISP_OFF,       0x09,    0x05, {0x59, 0x0D, 0x31, 0x02, 0x01}, 0x04, {0x91, 0x00, 0x00, 0x2E}},    // display off
//  { ACT_TEL,            0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x00, 0x00}, 0x01, {0x0C}}, // 0x13, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0xE3 }},
//  { ACT_TEL_CANCEL,     0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x00, 0x00}, 0x01, {0x05}}, // 0x13, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0xDC }},
//  { ACT_VOL,            0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x02, 0x00}, 0x00, {0x00}}  //0x05 = 1
//
//  // 10 680231020200 05 13000000001D0000DE - vol = 1
//};

const AvcInCmdTable  mtSearchHead[] PROGMEM = {
  { ACT_BUTTON_PRESS,   0x08,   0xBE},         // Button press
  { ACT_B_DISPOFF,      0x08,   0x2F},
  { ACT_B_DISPFULL,     0x08,   0xBD},
  { ACT_B_DISPHULF,     0x08,   0xCF},
  { ACT_CAM_ON,         0x09,   0x31},    // Cam ON
  { ACT_CAM_OFF,        0x09,   0x30},    // Cam OFF
  //        { ACT_DISP_HULF,      0x09,     0x30},    // display hulf color ???
  { ACT_DISP_OFF,       0x09,   0x2E},    // display off
  { ACT_TEL,            0x0A,   0xE3},
  { ACT_TEL_CANCEL,     0x0A,   0xDC}
  //        { ACT_VOL,            0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x02, 0x00}, 0x00, {0x00}}  //0x05 = 1
};

const byte mtSearchHeadSize = sizeof(mtSearchHead) / sizeof(AvcInCmdTable);
//char logBuff[40] = {0};

//--------------------------------------------------------------------------------
void printAvcAction( AvcActionID id )
//--------------------------------------------------------------------------------
{
  if (!bSDLog.bRedyToLog) return;

  char mBuff[20] = { 0 };
  sprintf( mBuff, "[%u]{%d}", millis(), id );
  bSDLog.logs( mBuff );
}

// AVCLan Navi & timer1 init,
//--------------------------------------------------------------------------------
void AVCLanHonda::begin()
//--------------------------------------------------------------------------------
{
  mVol = 0;
  avclan.deviceAddress = 0x0131;

  bShowHondaDisp = false;
  isHondaDisLast = false;
  bShowRearCam =  false;

  setWait( false );
  setWaitTime( 0L );
  setLockTime( 0L );

  // timer1 setup, prescaler factor - 1024
  TCCR1A = 0;       // normal mode
  TCCR1B = 5;       // Prescaler /1024
  TCNT1H = TI1_H;   // Load counter value hi
  TCNT1L = TI1_L;   // Load counter value lo
  DISABLE_TIMER1_INT;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setWaitTime( const unsigned long mTime )
//--------------------------------------------------------------------------------
{
  waitTime = mTime;
  if ( mTime > 0L ) {
    setWait(true);
  } else {
    setWait(false);
  }
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setLockTime( const unsigned long mTime )
//--------------------------------------------------------------------------------
{
  lockTime = mTime;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setWait( const bool mWait )
//--------------------------------------------------------------------------------
{
  bWait = mWait;
}

// Use the last received message to determine the corresponding action ID, store it in avclan object
//--------------------------------------------------------------------------------
void AVCLanHonda::getActionID()
//--------------------------------------------------------------------------------
{
  avclan.actionID = avclan.getActionID(mtSearchHead, mtSearchHeadSize);
};

// process action
//--------------------------------------------------------------------------------
void AVCLanHonda::processAction(AvcActionID ActionID)
//--------------------------------------------------------------------------------
{
  if ( (20000 > millis()) && (ACT_CAM_ON == ActionID) ) {
    bShowRearCam = true;
    isHondaDisLast = false;
    bShowHondaDisp = true;
    printAvcAction(ActionID);
    return;
  }

  switch ( ActionID ) {
    case ACT_BUTTON_PRESS:
      {
        printAvcAction(ActionID);
        if ( !bShowRearCam || !isWait() || ( getLockTime() < millis()) )
        {
          setWaitTime( (unsigned long)((millis() + BUTT_WAIT)) );
        }
      }
      break;
    case ACT_CAM_ON:
      bShowRearCam = true;
      isHondaDisLast = isShowHondaDisp();
      bShowHondaDisp = true;
      printAvcAction(ActionID);
      setLockTime( (unsigned long)(millis() + LOCK_TIME) );
      break;
    case ACT_DISP_OFF:
      if ( !bShowRearCam && isWait() )
      {
        bShowHondaDisp = false;
        isHondaDisLast = false;
      }
      printAvcAction(ActionID);
      setLockTime( (unsigned long)(millis() + LOCK_TIME) );
      break;
    case ACT_CAM_OFF:
      bShowRearCam = false;
      bShowHondaDisp = isHondaDisLast;
      printAvcAction(ActionID);
      setLockTime( (unsigned long)(millis() + LOCK_TIME) );
      break;
  }
};

//--------------------------------------------------------------------------------
void AVCLanHonda::checkWait()
//--------------------------------------------------------------------------------
{
  if ( isWait() && (getWaitTime() < millis()) ) {
    setWaitTime( 0L );
  }
}

//--------------------------------------------------------------------------------
bool AVCLanHonda::getCommute()
//--------------------------------------------------------------------------------
{
  return (bShowHondaDisp || bShowRearCam);
}

//--------------------------------------------------------------------------------
void AVCLanHonda::tryToShowHondaDisp()
//--------------------------------------------------------------------------------
{
  bWait = false;
  if ( getLockTime() > millis() ) return;
  bShowHondaDisp = true;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::falseHondaDis()
//--------------------------------------------------------------------------------
{
  bShowHondaDisp = false;
  isHondaDisLast = false;
}

AVCLanHonda avclanHonda;
