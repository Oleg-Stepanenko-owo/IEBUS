//--------------------------------------------------------------------------------
#include "limits.h"
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"

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

// { action_name,  packed_size, end_off_packege_word(check_sum)  }
const AvcInCmdTable  mtSearchHead[] PROGMEM = {
  { ACT_BUTTON_PRESS,   0x08,   0xBE},     // Button press
  { ACT_B_DISPOFF,      0x08,   0x2F},
  { ACT_B_DISPFULL,     0x08,   0xBD},
  { ACT_B_DISPHULF,     0x08,   0xCF},
  { ACT_CAM_ON,         0x09,   0x31},    // Cam ON
  { ACT_CAM_OFF,        0x09,   0x30},    // Cam OFF
  //        { ACT_DISP_HULF,      0x09,     0x30},    // display hulf color ???
  { ACT_DISP_OFF,       0x09,   0x2E},    // display off
  { ACT_TEL,            0x0A,   0xE3},	  // start Tel Action
  { ACT_TEL_CANCEL,     0x0A,   0xDC}	  // Cancel or End Call_ACTION
  //        { ACT_VOL,            0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x02, 0x00}, 0x00, {0x00}}  //0x05 = 1
};

const byte mtSearchHeadSize = sizeof(mtSearchHead) / sizeof(AvcInCmdTable);

//--------------------------------------------------------------------------------
void AVCLanHonda::begin()
//--------------------------------------------------------------------------------
{
  avclan.deviceAddress = 0x0131;

  bShowHondaDisp = false;
  bHondaDisLast = false;
  bShowRearCam =  false;
  bFirstStart_20 = true;
  bFreeze = false;

  setWaitTime( 0L );
  freezeTime = 0L;

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
  bWait = (bool) waitTime;
}

// Use the last received message to determine the corresponding action ID,
// store it in avclan object
//--------------------------------------------------------------------------------
void AVCLanHonda::getActionID()
//--------------------------------------------------------------------------------
{
  avclan.actionID = avclan.getActionID( mtSearchHead, mtSearchHeadSize );
};

// process action
//--------------------------------------------------------------------------------
void AVCLanHonda::processAction( AvcActionID ActionID )
//--------------------------------------------------------------------------------
{
  if ( bFirstStart_20 && (20000 > millis()) && (ACT_CAM_ON == ActionID) ) {
    bShowRearCam = true;
    bHondaDisLast = false; //after reverse action should be GVN screen
    bShowHondaDisp = true;
    return;
  }

  if ( bFirstStart_20 && (20000 < millis()) ) bFirstStart_20 = false;

  switch ( ActionID ) {
    case ACT_BUTTON_PRESS:
      {
        if ( !bShowRearCam || !isWait() )
        {
          if ( ULONG_MAX > (millis() + BUTT_WAIT) )
            setWaitTime( (unsigned long)((millis() + BUTT_WAIT)) );
          else setWaitTime( BUTT_WAIT );
        }
      }
      break;
    case ACT_CAM_ON:
      bShowRearCam = true;
      bHondaDisLast = isShowHondaDisp();
      bShowHondaDisp = true;
      setWaitTime(0L);
      break;
    case ACT_DISP_OFF:
      if ( !bShowRearCam )
      {
        // need freeze on 2000 sec after code receiving.
        bShowHondaDisp = false;
        bHondaDisLast = false;
        setWaitTime(0L);
        bFreeze = true;
        freezeTime = (millis() + FREEZE_TIME);
      }
      break;
    case ACT_CAM_OFF:
      bShowRearCam = false;
      bShowHondaDisp = bHondaDisLast;
      setWaitTime(0L);
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
  bHondaDisLast = bShowHondaDisp;
  bShowHondaDisp = true;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setHondaDis( bool val )
//--------------------------------------------------------------------------------
{
  bShowHondaDisp = val;
  bHondaDisLast = val;
  setWaitTime(0L);
}


AVCLanHonda avclanHonda;


