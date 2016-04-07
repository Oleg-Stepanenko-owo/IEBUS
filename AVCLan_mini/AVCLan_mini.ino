
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

static int MAX_ERROR_COUNT = 30;
byte errorID;
int  error_count;
char BUFFF[15];

char buf[12]; //buffer to store AT commands
int len = 0; //stores the length of the commands
//--------------------------------------------------------------------------------
void setup()
//--------------------------------------------------------------------------------
{
  // Serial.begin(9600);
  avclan.begin();
  avclanHonda.begin();
  errorID = 0;
  error_count = 0;

  avclanBT.begin();
  avclanBT.print("Start HONDA avclan.#");
  sprintf( BUFFF, "%s", IEBUS_VERSION );
  avclanBT.println( BUFFF );

  LED_ON;
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

  if ( avclanHonda.bFirstStart_20  && !avclanHonda.isShowRearCam() && (11500 < millis()) )
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

      avclan.printMessage(true);
      error_count = 0;
      avclanHonda.getActionID();

      if ( avclanHonda.bFirstStart_20 && (avclan.actionID == ACT_CAM_ON) )
      {
        avclanBT.printAction((AvcActionID)avclan.actionID);
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
      else if ( (avclan.actionID != ACT_NONE) && ( 20000 < millis() ) && (!avclanHonda.bFreeze))
      {
        avclanBT.printAction((AvcActionID)avclan.actionID);
        avclanHonda.processAction( (AvcActionID)avclan.actionID );
      }
      else
      {
        if ( avclanHonda.bFirstStart_20 ) avclanBT.println(">>FIRST_SKIP<<");
        else if ( avclanHonda.bFreeze ) avclanBT.println(">>FREEZE_SKIP<<");
      }
      LED_ON;
    }
    else
    {
      if ( errorID == res ) error_count++;
      else error_count = 1;

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

  if ( error_count > MAX_ERROR_COUNT ) {
    avclanHonda.bFirstStart_20 = false;
    error_count = 0;
    avclanHonda.setHondaDis(true);

    LED_OFF;
    HONDA_DIS_ON;

    sprintf(BUFFF, "Error: %d", errorID );
    avclanBT.println( BUFFF );
    //avclan.begin();
    //cli();
  }
}



