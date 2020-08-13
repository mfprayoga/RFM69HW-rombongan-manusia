 #include <RFM69.h>      
#include <RFM69_ATC.h>    
#include <SPIFlash.h>     
#include <SPI.h>     
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
     
#define NODEID        1    //Node ID yang digunakan, Gateway = 1
#define NETWORKID     100  //Network ID yang digunakan, rentang 2 sampai 255
#define FREQUENCY     RF69_433MHZ // Frekuensi yang digunakan, Bandwidth 1Hz)
#define ENCRYPTKEY    "syauqiEncryptKey" //Kode Enkripsi 16bit kombinasi
#define IS_RFM69HW_HCW //Jenis Modul RFM
#define ENABLE_ATC    //meng-enablekan Automatic Trasmision Control
#define SERIAL_BAUD   115200


#if defined (MOTEINO_M0) && defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards

#endif

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

bool promiscuousMode = false; 
byte ackCount = 0;
uint32_t packetCount = 0;

unsigned long currentMillis;
unsigned long node1_last, node2_last, node3_last, node4_last, node5_last;
unsigned long node_periode = 7000;

String notif[5];
int urutan = 0;

void(* reset)(void) = 0; 

void setup() {
  delay(501);
  pinMode (10, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (3, OUTPUT);
  digitalWrite (10, LOW);
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); 
#endif
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  pinMode(8, OUTPUT);
  char buff[50];
}

void loop() {
  currentMillis = millis();
  digitalWrite (6, LOW);
  if((currentMillis - node1_last) >= node_periode) {
      updateNotif("A",1);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
    }
    else{
      updateNotif("A",0);
      digitalWrite (10, LOW);
    }
      if((currentMillis - node2_last) >= node_periode) {
      updateNotif("B",1);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
    }
    else{
      updateNotif("B",0);
      digitalWrite (10, LOW);
    }
      if((currentMillis - node3_last) >= node_periode) {
      updateNotif("C",1);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
    }
    else{
      updateNotif("C",0);
      digitalWrite (10, LOW);
    }
      if((currentMillis - node4_last) >= node_periode) {
      updateNotif("D",1);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
    }
    else{
      updateNotif("D",0);
      digitalWrite (10, LOW);
    }
      if((currentMillis - node5_last) >= node_periode) {
      updateNotif("E",1);
      digitalWrite (10, HIGH);
      digitalWrite (6, HIGH);
    }
    else{
      updateNotif("E",0);
      digitalWrite (10, LOW);
    }

    u8g.firstPage();  
     do {
    draw();
     } while( u8g.nextPage() );
     
  if (radio.receiveDone())
  {
    if (radio.DATALEN==2 && radio.DATA[0]=='A' && radio.DATA[1]=='A')
    {
      digitalWrite(6, HIGH);
      delay(100);
      digitalWrite(6, LOW);
      }

      if (radio.DATALEN==2 && radio.DATA[0]=='B' && radio.DATA[1]=='B')
    {
      digitalWrite(6, HIGH);
      delay(100);
      digitalWrite(6, LOW);
      }

      if (radio.DATALEN==2 && radio.DATA[0]=='C' && radio.DATA[1]=='C')
    {
      digitalWrite(6, HIGH);
      delay(100);
      digitalWrite(6, LOW);
      }
      
      if (radio.DATALEN==2 && radio.DATA[0]=='D' && radio.DATA[1]=='D')
    {
      digitalWrite(6, HIGH);
      delay(100);
      digitalWrite(6, LOW);
      }

      if (radio.DATALEN==2 && radio.DATA[0]=='E' && radio.DATA[1]=='E')
    {
      digitalWrite(6, HIGH);
      delay(100);
      digitalWrite(6, LOW);
      }
      
    Serial.print(++packetCount);
    Serial.print(" Node:[");
    Serial.print(radio.SENDERID, DEC);
    Serial.println("] ");

    int node_id = radio.SENDERID;
    switch(node_id){
      case 2:
        node1_last = currentMillis;
      break;
      case 3:
        node2_last = currentMillis;
      break;
      case 4:
        node3_last = currentMillis;
      break;
      case 5:
        node4_last = currentMillis;
      break;
      case 6:
        node5_last = currentMillis;
      break;
    }
    for (byte i = 0; i < radio.DATALEN; i++)
    Serial.print((char)radio.DATA[i]);
    Serial.print("   [RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    Serial.println();
    Blink(LED_BUILTIN,3);
  }
  if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
       Serial.print("ACK SENT to [");Serial.print(radio.SENDERID, DEC);Serial.print("] "); Serial.println();   
}
}

void updateNotif(String node, int state){
  if(state == 1){
    bool allow = true;

    for(int i = 0; i < urutan; i++){
      if(notif[i].equals(node)){
        allow = false;
      }
    }
    
    if(allow){
      if(urutan < 5){
          notif[urutan] = node;
        urutan++;
      }
    }
  }
  else if(state == 0){
    bool allow = false;

    for(int i = 0; i < urutan; i++){
      if(notif[i].equals(node)){
        allow = true;
      }
    }

    if(allow){
      if(urutan > 0){
          int found = 0;
          for(int i = 0; i < urutan; i++){
            if(notif[i].equals(node)){
              found = i;
            }
            break;
          }
          notif[found] = "";
          String temp[5];
          int tambah = 0;
          for(int j = 0; j < urutan; j++){
            if(!notif[j].equals("")){
              temp[tambah] = notif[j];
              tambah++;
            }
          }
  
          for(int k = 0; k < tambah; k++){
            notif[k] = temp[k];
          }
        urutan--;
      }
    }
  }
}

void draw(void) {
  String forshow = "";
  for(int i = 0; i < urutan; i++){
    forshow += notif[i];
    forshow += " ";
  }
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 10, "Rombongan Grup A");
  u8g.drawStr( 0, 30, forshow.c_str());
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

void showNotif(){
  for(int i = 0; i < urutan; i++){
    Serial.print(notif[i]);
    Serial.print(" ");
  }
  Serial.println("");
}
