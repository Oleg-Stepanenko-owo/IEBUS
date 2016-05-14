
//--------------------------------------------------------------------------------
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
#include "AVCLan_BT.h"
#include "config.h"
//--------------------------------------------------------------------------------

#define LED_ON   sbi(LED_PORT, LED_OUT);
#define LED_OFF  cbi(LED_PORT, LED_OUT);

#define HONDA_DIS_ON   sbi(COMMUT_PORT, COMMUT_OUT);
#define HONDA_DIS_OFF  cbi(COMMUT_PORT, COMMUT_OUT);

char BUFFF[15];

//------ Error time updating -----------------------------------------------------
// unsigned long waitErrorTime;
bool beforeErrorComute;
static int MAX_ERROR_COUNT = 10;
byte errorID;
int  error_count;

#define ERROR_TIME 4000

// ERRORs-------------------------------------------------------------
// #1 - First 20 sec start, should be GVN show
// #2 - Set setHondaDis value true
// #3 - HONDA DISP ON
// #4 - HONDA DISP OFF
// #5 - after 5 sec honda logo show GVN
// #6 - reset Freeze time
// #7 - set rear cam in first 10 sek
// #8 - processAction : show rear cam in first 20 sek
// #9 -  before was action for preparing cam off and now cam DISPOFF action
// #10 - set wait time
// #11 - set wait time 0
// #12 - default action updating
// #13 - first 20 sek we should react only on rear cam on/off
// #14 - so many error comes
// #15 - after wait try to show honda display
// #16 - Set setHondaDis value false

//--------------------------------------------------------------------------------
void setup()
//--------------------------------------------------------------------------------
{
  HONDA_DIS_ON;
  LED_ON;

  // Serial.begin(9600);
  avclan.begin();
  avclanHonda.begin();
  errorID = 0;
  error_count = 0;
  beforeErrorComute = 0;
  // waitErrorTime = 0;

  avclanBT.begin();
  sprintf( BUFFF, "Start Ver:%s", IEBUS_VERSION );
  avclanBT.println( BUFFF, true );
  avclanHonda.setHondaDis( true );
}

//--------------------------------------------------------------------------------
void loop()
//--------------------------------------------------------------------------------
{
  if ( avclanBT.available() )
  {
    int len = avclanBT.available();
    for (int i = 0; i < len; i++) avclanBT.checkCommand(avclanBT.read());
  }

  if( 1 == avclanBT.getDisplayStatus() ){
        HONDA_DIS_ON;
        LED_ON;
  } else if ( 2 == avclanBT.getDisplayStatus() ){
        HONDA_DIS_OFF;
        LED_OFF;
  }

  // First 5 sec should be Honda logo
  if ( !avclanHonda.isShowHondaDisp() && avclanHonda.bFirstStart_20 && (INIT_TIME > millis()) ) {
    avclanBT.println("#1");
    avclanHonda.setHondaDis( true ); //Show GVN screen
  } else if ( avclanHonda.isShowHondaDisp() && !avclanHonda.isShowRearCam()
              && (INIT_TIME < millis()) && (INIT2_TIME > millis()) && avclanHonda.bFirstStart_20 ) {
    avclanBT.println("#5");
    avclanHonda.setHondaDis( false );
  }

  if ( avclanHonda.bFreeze ) {
    if ( avclanHonda.freezeTime < millis() ) {
      avclanHonda.bFreeze = false;
      avclanHonda.freezeTime = 0L;
      avclanBT.print("#6");
    }
  }

  if ( INPUT_IS_SET ) {
    byte res = avclan.readMessage();
    if ( !res )
    {
      // LED_OFF;
      error_count = 0;

      avclan.printMessage(true);
      avclanHonda.getActionID();

      if ( avclan.actionID != ACT_NONE )
      {
        if ( (INIT2_TIME > millis()) && (avclan.actionID == ACT_CAM_ON) )
        {
          avclanBT.println("#7");
          avclanBT.printAction( (AvcActionID)avclan.actionID );
          avclanHonda.processAction( (AvcActionID)avclan.actionID );
        }
        else if ( ( INIT2_TIME < millis() ) && (!avclanHonda.bFreeze) )
        {
          avclanBT.println("#12");
          avclanBT.printAction((AvcActionID)avclan.actionID);
          avclanHonda.processAction( (AvcActionID)avclan.actionID );
        }
        else // first 20 sek we should react only on rear cam on/off
        {
          avclanBT.println("#13");
          avclanBT.printAction( (AvcActionID)avclan.actionID );
        }
      }

      // LED_ON;

      // back to screeen that was before Error:
      if ( error_count >= MAX_ERROR_COUNT )
      {
        avclanBT.println("#14");
        error_count = 0;
        if ( !avclanHonda.isShowRearCam() )
          avclanHonda.setHondaDis( beforeErrorComute );
        else avclanHonda.setHondaDisLast( false ); //after error and rear cam off - try to show GVN display
      }
    }
    else
    {
      if ( errorID == res ) error_count++;
      else error_count = 0;

      errorID = res;
    }
  }

  if ( avclanHonda.isWait() )
  {
    avclanHonda.checkWait();
    if ( !avclanHonda.isWait() ) {
      avclanBT.println("#15");
      avclanHonda.tryToShowHondaDisp();
    }
  }
  else
  {
    if ( avclanHonda.getCommute() )
    {
      HONDA_DIS_ON;
      LED_ON;
      // avclanBT.println("#3");
    }
    else
    {
      HONDA_DIS_OFF;
      LED_OFF;
      // avclanBT.println("#4");
    }
  }

  //------- ERROR CHECKING BLOCK ----------------------------------
  if ( (error_count > MAX_ERROR_COUNT) && !avclanHonda.isShowHondaDisp() ) {
    //    avclanHonda.bFirstStart_20 = false;
    error_count = 0;

    beforeErrorComute = avclanHonda.getCommute();
    avclanHonda.setHondaDis(true); // Show Honda display

    LED_ON;
    HONDA_DIS_ON;
    avclanBT.println("#3");

    sprintf(BUFFF, "E:%d", errorID );
    avclanBT.println( BUFFF, true );
  }
}



