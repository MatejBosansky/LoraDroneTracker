
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)

//Lora Configuration, keep same settings for both projects:
#define BAND 868E6 // NA - 915E6 / EU - 868E6
#define SpreadFactor 12 //7-12 higher - better range, lower speed
#define BandWidth 41700 //oposite as Spread factor, available in kHz: 7.8 10.4 15.6 20.8 31.2 41.7 62.5 125 250 500, be careful with lowest value, it has only 19bps speed
#define CodingRate 8 //5-8 higher - more transmition time but less corrupted messages
#define SyncWord 42 //0-255 - something like private channel to not interrupt other lora devices

#define Serial_Output
#define BT_Output //comment line if you dont want use Bluetooth

void PrintMsg(const String& Msg);
void PrintMsgln(const String& Msg);
char LocationMessage[200]; 

void setup() {
  Serial.begin(115200);
  
  #ifdef BT_Output
  SerialBT.begin("ESP32test"); //Bluetooth device name
  #endif

  while (!Serial);

  PrintMsgln("LoRa Receiver");

  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (BAND)) {
    PrintMsgln("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(SpreadFactor); 
  LoRa.setSignalBandwidth(BandWidth);
  LoRa.setCodingRate4(CodingRate);
  LoRa.setSyncWord(SyncWord);
}



void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    PrintMsg("http://maps.google.com/maps?q=");
    while (LoRa.available()) {
      PrintMsg(String((char)LoRa.read()));
    }
    PrintMsg(" Packet. RSSI ");
    PrintMsg(String(LoRa.packetRssi()));
    PrintMsg(" SNR ");
    PrintMsg(String(LoRa.packetSnr()));
    PrintMsg(" Error freq ");
    PrintMsgln(String(LoRa.packetFrequencyError()));
  }
}

void PrintMsg(const String& Msg){
#ifdef Serial_Output
Serial.print(Msg);
#endif

#ifdef BT_Output
  SerialBT.print(Msg);
#endif
}

void PrintMsgln(const String& Msg){
#ifdef Serial_Output
Serial.println(Msg);
#endif

#ifdef BT_Output
  SerialBT.println(Msg);
#endif
}