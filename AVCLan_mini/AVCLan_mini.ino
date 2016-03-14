
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
#include "config.h"
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#define LED_ON   sbi(LED_PORT, LED_OUT);
#define LED_OFF   cbi(LED_PORT, LED_OUT);

#define HONDA_DIS_ON   sbi(LED_PORT, COMMUT_OUT);
#define HONDA_DIS_OFF  cbi(LED_PORT, COMMUT_OUT);

//--------------------------------------------------------------------------------
void setup()
//--------------------------------------------------------------------------------
{
  sbi(LED_DDR,  COMMUT_OUT);
  cbi(LED_PORT, COMMUT_OUT);

  avclan.begin();
  avclanHonda.begin();
}

//--------------------------------------------------------------------------------
void loop()
//--------------------------------------------------------------------------------
{
  if ( avclanHonda.bFirstStart_20 && (11500 > millis()) ) {
    HONDA_DIS_ON;  // initalize
    return;
  } else if ( avclanHonda.bFirstStart_20  && !avclanHonda.isShowRearCam() && (11500 < millis()) ) {
    avclanHonda.setHondaDis( false ); //Show GVN screen
  }

  if ( INPUT_IS_SET ) {
    byte res = avclan.readMessage();
    if ( !res ) {
      avclanHonda.getActionID();
      if ( avclan.actionID != ACT_NONE ) {
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
    }
  }

  if ( avclanHonda.isWait() ) {
    avclanHonda.checkWait();
    if ( !avclanHonda.isWait() ) avclanHonda.tryToShowHondaDisp();
  } else {
    if ( avclanHonda.getCommute() ) {
      HONDA_DIS_ON;
    } else {
      HONDA_DIS_OFF;
    }
  }
}



