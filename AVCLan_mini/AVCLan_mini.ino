
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
#include "AVCLan_BT.h"
#include "config.h"
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#define LED_ON   sbi(LED_PORT, LED_OUT);
#define LED_OFF  cbi(LED_PORT, LED_OUT);

#define HONDA_DIS_ON   sbi(COMMUT_PORT, COMMUT_OUT);
#define HONDA_DIS_OFF  cbi(COMMUT_PORT, COMMUT_OUT);

static int MAX_ERROR_COUNT = 30;
byte errorID;
int  error_count;
char BUFFF[15];
//--------------------------------------------------------------------------------
void setup()
//--------------------------------------------------------------------------------
{
  avclan.begin();
  avclanHonda.begin();
  errorID = 0;
  error_count = 0;

  avclanBT.begin();
  avclanBT.println("Start HONDA avclan.");
  LED_ON;
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

  if ( avclanHonda.bFreeze ) {
    if ( avclanHonda.freezeTime < millis() )  {
      avclanHonda.bFreeze = false;
      avclanHonda.freezeTime = 0L;
    } else return;
  }

  if ( INPUT_IS_SET ) {
    byte res = avclan.readMessage();
    if ( !res ) {
      error_count = 0;

      avclanHonda.getActionID();
      if ( avclan.actionID != ACT_NONE ) {
        sprintf( BUFFF, "Action: %d", avclan.actionID );
        avclanBT.println( BUFFF );

        LED_ON;
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
    } else {
      if ( errorID == res ) error_count++;
      else error_count = 1;

      errorID = res;
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

  if ( error_count > MAX_ERROR_COUNT ) {
    error_count = 0;
    avclanHonda.setHondaDis(true);

    LED_OFF;

    sprintf(BUFFF, "Error: %d", errorID );
    avclanBT.println( BUFFF );
  }
}



