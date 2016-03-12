
//--------------------------------------------------------------------------------
#include "AVCLanDrv.h"
#include "BuffSerial.h"
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

  bSerial.begin(250000);
  avclan.begin();
}

//--------------------------------------------------------------------------------
void loop()
//--------------------------------------------------------------------------------
{
  if (INPUT_IS_SET) {
    byte res = avclan.readMessage();
    if (!res) {
      avclan.printMessage(true);
    } else {
      bSerial.print("!");
      bSerial.printHex4(res);
      bSerial.println();
      while (!avclan.isAvcBusFree());
    }
  }
}

////--------------------------------------------------------------------------------
//void EERPOM_read_config()
////--------------------------------------------------------------------------------
//{
//  if (EEPROM.read(E_INIT) != 'T')
//  {
//    EEPROM.write(E_MASTER1, 0x01);
//    EEPROM.write(E_MASTER2, 0x31);
//    EEPROM.write(E_READONLY, 0);
//    EEPROM.write(E_INIT, 'T');
//  }
//  else
//  {
//    avclan.headAddress = (EEPROM.read(E_MASTER1) << 8) + EEPROM.read(E_MASTER2);
//    avclan.readonly    =  EEPROM.read(E_READONLY);
//  }
//}



