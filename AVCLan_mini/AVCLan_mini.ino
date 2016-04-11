
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
int len = 0; //stores the length of the commands

//------ Error time updating -----------------------------------------------------
// unsigned long waitErrorTime;
bool beforeErrorComute;
static int MAX_ERROR_COUNT = 10;
byte errorID;
int  error_count;

#define ERROR_TIME 4000

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
  avclanBT.print("Start HONDA avclan.#");
  sprintf( BUFFF, "%s", IEBUS_VERSION );
  avclanBT.println( BUFFF );
}

//--------------------------------------------------------------------------------
void loop()
//--------------------------------------------------------------------------------
{
  if ( avclanBT.available() )
  {
    len = avclanBT.available();
    for (int i = 0; i < len; i++) avclanBT.checkCommand(avclanBT.read());
  }

  if ( avclanHonda.bFirstStart_20  && !avclanHonda.isShowRearCam() && (INIT_TIME < millis()) )
  {
    avclanHonda.setHondaDis( false ); //Show GVN screen
  }

  if ( avclanHonda.bFreeze ) {
    if ( avclanHonda.freezeTime < millis() ) {
      avclanHonda.bFreeze = false;
      avclanHonda.freezeTime = 0L;
    }
  }

  if ( INPUT_IS_SET ) {
    byte res = avclan.readMessage();
    if ( !res )
    {
      LED_OFF;
      error_count = 0;

      avclan.printMessage(true);
      avclanHonda.getActionID();

      if ( avclanHonda.bFirstStart_20 && (avclan.actionID == ACT_CAM_ON) )
      {
        avclanBT.printAction( (AvcActionID)avclan.actionID );
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
      else if ( (avclan.actionID != ACT_NONE) && ( INIT2_TIME < millis() ) && (!avclanHonda.bFreeze))
      {
        avclanBT.printAction((AvcActionID)avclan.actionID);
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
      else // first 20 sek we should react only on rear cam on/off
      {
        // if ( avclanHonda.bFirstStart_20 ) avclanBT.println(">>FIRST_SKIP<<");
        // else if ( avclanHonda.bFreeze ) avclanBT.println(">>FREEZE_SKIP<<");
      }

      LED_ON;

      // back to screeen that was before Error:
      if ( error_count >= MAX_ERROR_COUNT )
      {
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
    if ( !avclanHonda.isWait() ) avclanHonda.tryToShowHondaDisp();
  }
  else
  {
    if ( avclanHonda.getCommute() )
    {
      HONDA_DIS_ON;
    }
    else
    {
      HONDA_DIS_OFF;
    }
  }

  //------- ERROR CHECKING BLOCK ----------------------------------
  if ( (error_count > MAX_ERROR_COUNT) && !avclanHonda.isShowHondaDisp() ) {
//    avclanHonda.bFirstStart_20 = false;
    error_count = 0;

    beforeErrorComute = avclanHonda.getCommute();
    avclanHonda.setHondaDis(true); // Show Honda display

    LED_OFF;
    HONDA_DIS_ON;

    sprintf(BUFFF, "Error: %d", errorID );
    avclanBT.println( BUFFF );
  }
}



