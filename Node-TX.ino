// **********************************************************************************

#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>      //get it here: https://www.github.com/lowpowerlab/spiflash
#include <SPI.h>           

#define NODEID        2    // Node ID yang digunakan, rentang 2 sampai 255
#define NETWORKID     100  // Network ID yang digunakan, rentang 2 sampai 255
#define GATEWAYID     1    // Gateway ID yang digunakan

#define FREQUENCY   RF69_433MHZ     // Frekuensi yang digunakan, Bandwidth 1Hz
#define ENCRYPTKEY  "EncryptKey"    // Kode Enkripsi 16bit kombinasi
#define IS_RFM69HW_HCW              // Jenis Modul RFM

#define ENABLE_ATC  //mengaktifkan Automatic Transmission Control
#define ATC_RSSI    -80

#define SERIAL_BAUD 115200

#if defined (MOTEINO_M0) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards
#endif

int TRANSMITPERIOD = 200;           // Mengirimkan data ke gateway dalam 200ms
char payload[] = "Grup A";
byte sendSize = strlen(payload);    // Menghitung jumlah karakter variabel payload
boolean requestACK = false;

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void setup() {
  pinMode(10, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(3, INPUT);
  digitalWrite(10, LOW);
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  #ifdef IS_RFM69HW_HCW
    radio.setHighPower(); 
  #endif
  radio.encrypt(ENCRYPTKEY);

  #ifdef ENABLE_ATC
    radio.enableAutoPower(ATC_RSSI);
  #endif

  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
}

long lastPeriod = 0;
void loop() {

  //check for any received packets (ACK from gateway)
  /*
  if (radio.receiveDone()) {
    Serial.print("[");
    Serial.print(radio.SENDERID, DEC);
    Serial.print("] ");
    for (byte index = 0; index < radio.DATALEN; index++)
      Serial.print((char)radio.DATA[index]);

    Blink(LED_BUILTIN, 3);
    Serial.println();
  }
  */

  int currPeriod = millis()/TRANSMITPERIOD;
  int interruptPin = 3;
  int readInterrupt = (digitalRead(interruptPin));
  bool emerButton = false;

  if (readInterrupt == HIGH)            // Mengubah nilai emerButton menjadi TRUE jika tombol interrupt ditekan
    emerButton = true;

  if (currPeriod != lastPeriod) {
    lastPeriod = currPeriod;

    if(sendSize != 0) {
      Serial.print("Sending[");
      Serial.print(NODEID);
      Serial.print("]: ");

      // for(byte index = 0; index < sendSize; index++)
      //   Serial.print((char)payload[index]);

      Serial.print(payload);

      if (emerButton) {
        radio.sendWithRetry(GATEWAYID, "Emergency", 9);         // Jika emerButton = TRUE, maka pesan yang dikirim adalah Emergency
        Serial.print(" Emergency!");

        emerButton = false;                                     // emerButton akan dikembalikan ke kondisi awal = tombol dilepas
      } else {
        if (radio.sendWithRetry(GATEWAYID, payload, sendSize)) {
          Serial.print("! Connect");
          digitalWrite(6, LOW);
        } else {
          Serial.print("! Disconnect");
          digitalWrite(6, HIGH);
        }
      }
    }

    // sendSize = (sendSize + 1) % 31;
    Serial.println();
    Blink(LED_BUILTIN, 3);
  }
}
