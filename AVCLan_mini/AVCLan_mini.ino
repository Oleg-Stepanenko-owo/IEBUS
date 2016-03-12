
//--------------------------------------------------------------------------------
#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
//#include "BuffSerial.h"
#include "SD_Log.h"
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

  // bSDLog.begin();

  //  Serial.begin(250000);
  //  if (Serial) {
  //    Serial.println("Start HONDA");
  //  }

  avclan.begin();
  avclanHonda.begin();

  EERPOM_read_config();
  //  bSDLog.testLog();
}

//--------------------------------------------------------------------------------
void loop()
//--------------------------------------------------------------------------------
{

  if ( avclanHonda.bFirstStart_20 && (11500 > millis()) ) {
    HONDA_DIS_ON;  // initalize
    return;
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


  //-------------------------------

  /*

    if ( !avclanHonda.isLockTime() )
    {
      if ( avclanHonda.isWait() ) {
        avclanHonda.checkWait();
        if (avclanHonda.isWait() == false)
          avclanHonda.tryToShowHondaDisp();
      }

      if ( INPUT_IS_SET ) {
        byte res = avclan.readMessage();
        if ( !res ) {
          //LOG
          //      avclan.printMessage(true);
          avclanHonda.getActionID();
          if ( avclan.actionID != ACT_NONE ) {
            avclanHonda.processAction( (AvcActionID)avclan.actionID );
          }
        }
      }
    }
    else
    {
      //ON a start target
      if ( 11500 > millis() ) {
        HONDA_DIS_ON;  // initalize
        return;
      } else if ( 18000 > millis() ) {
        avclanHonda.falseHondaDis();
        if ( !avclanHonda.isShowRearCam() ) {
          HONDA_DIS_OFF;
          return;
        } else {
          HONDA_DIS_ON;
          return;
        }
      } else if ( avclanHonda.getCommute() ) {
        HONDA_DIS_ON;
      } else {
        HONDA_DIS_OFF;
      }
    }

  */
}

//--------------------------------------------------------------------------------
void EERPOM_read_config()
//--------------------------------------------------------------------------------
{
  if (EEPROM.read(E_INIT) != 'T')
  {
    EEPROM.write(E_MASTER1, 0x01);
    EEPROM.write(E_MASTER2, 0x31);
    EEPROM.write(E_READONLY, 0);
    EEPROM.write(E_INIT, 'T');
  }
  else
  {
    avclan.headAddress = (EEPROM.read(E_MASTER1) << 8) + EEPROM.read(E_MASTER2);
    avclan.readonly    = EEPROM.read(E_READONLY);
  }
}



