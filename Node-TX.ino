#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>      //get it here: https://www.github.com/lowpowerlab/spiflash

#define NODEID        6    // keep UNIQUE for each node on same network
#define NETWORKID     100  // keep IDENTICAL on all nodes that talk to each other
#define GATEWAYID     1    // "central" node
#define FREQUENCY   RF69_433MHZ
#define ENCRYPTKEY    "syauqiEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI      -80
#define SERIAL_BAUD   115200

#if defined (MOTEINO_M0) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards
#endif

int TRANSMITPERIOD = 200; //transmit a packet to gateway so often (in ms)
char payload[] = "Syauqi Data Yang Dikirim";
char buff[20];
byte sendSize=0;
boolean requestACK = false;

const int buttonPin = 3;
int buttonState = 0;


#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void setup() {
  pinMode (10, OUTPUT);
  pinMode (6, OUTPUT);
  digitalWrite (10, LOW);
  digitalWrite (6, LOW);
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif

#ifdef ENCRYPTKEY
  radio.encrypt(ENCRYPTKEY);
#endif
}

long lastPeriod = 0;
void loop() {
  if (radio.sendWithRetry(GATEWAYID, payload, sendSize)){
  Serial.println("connect");
  digitalWrite(6, LOW);
  } else {
    Serial.println("disconnect");
    digitalWrite(6, HIGH);
  }
  
  int currPeriod = millis()/TRANSMITPERIOD;
  const int buttonPin = 3;
  int buttonState = 0; 
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    (radio.sendWithRetry(GATEWAYID, "EE", 2));
    Serial.println("Button Emergency");
  }
}