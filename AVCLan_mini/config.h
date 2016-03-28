/*
  config.h  - AVCLan driver config
  Created by Kochetkov Aleksey, 04.08.2010
*/
#define IEBUS_VERSION "0.4"

// type of AVCLan hardwire driver
#define AVCLAN_PCA82C250

// maximum number of attempts to send a message
#define MAXSENDATTEMP   3

//********** arduino & driver based on PCA82C250 **********
#ifdef AVCLAN_PCA82C250
#define AVCLANDRIVER_TYPE    "Arduino - PCA82C250"
// define out pin (pin D6 arduino)
#define  DATAOUT_DDR    DDRD
#define  DATAOUT_PORT   PORTD
#define  DATAOUT_PIN    PIND
#define  DATAOUT        6

// define in pin (pin D5 arduino)
#define  DATAIN_DDR  DDRD
#define  DATAIN_PORT PORTD
#define  DATAIN_PIN  PIND
#define  DATAIN      5

// LED connected to digital pin 13
#define  LED_DDR     DDRC
#define  LED_PORT    PORTC
#define  LED_PIN     PINC
#define COMMUT_OUT   0


// AZFM board activate
#define AZFM_INIT
#define AZFM_ON
#define AZFM_OFF

#endif





