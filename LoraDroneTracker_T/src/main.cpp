#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <TinyGPS++.h>

//Pinout
#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define HW_UART_RX 16 //GPS TX goes to ESP32s RX
#define HW_UART_TX 17 //GPS RX goes to ESP32s TX

//Lora Configuration, keep same settings for both projects:
#define BAND 868E6 // for NA 915E6 
#define SpreadFactor 12 //7-12 higher - better range, lower speed
#define BandWidth 41700 //oposite as Spread factor, available in kHz: 7.8 10.4 15.6 20.8 31.2 41.7 62.5 125 250 500, be careful with lowest value, it has only 19bps speed
#define CodingRate 8 //5-8 higher - more transmition time but more stable message validity
#define SyncWord 42 //0-255 - something like private channel to not interrupt other lora devices
//Transmitter specific
#define TxPower 14 //2-20, higher better strenght, higher consuption
#define GPS_Baud 9600 //most of GPS modules have 9600
#define StartDelay 1 //in s, with this, you can start transmit location after specific period of time from start. 
#define TransmitInterval 10 //Interval in s between every transmition of location 


char LocationMessage[200]; //variable with Location message

void GenerateMessage(); //Collect data from gps and prepare message to send to char LocationMessage
void UBLOX_GPS_Shutdown(); //Sleep GPS to reduce power consuption durring deep sleep mode
void UBLOX_GPS_Wakeup(); //Turn on GPS
int counter = 0;
TinyGPSPlus gps;


void setup() {
  Serial.begin(115200);
  Serial2.begin(GPS_Baud, SERIAL_8N1, HW_UART_RX, HW_UART_TX);
  
  UBLOX_GPS_Shutdown();
  esp_sleep_enable_timer_wakeup(StartDelay*1000000);
    
  Serial.println("Entering sleep mode for " + String(StartDelay) + String(" seconds") );
  delay(1000);
  esp_light_sleep_start();
  Serial.print("Waking up from sleep mode");
  UBLOX_GPS_Wakeup();
  
  while (!Serial);

  Serial.println("LoRa Sender");

  SPI.begin (SCK, MISO, MOSI, SS);
  LoRa.setPins (SS, RST, DI0);
  if (! LoRa.begin (BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

LoRa.setSpreadingFactor(SpreadFactor); //7-12 highest - better range, lower speed
LoRa.setSignalBandwidth(BandWidth);
LoRa.setCodingRate4(CodingRate);
//some boards has different pin
//LoRa.setTxPower(TxPower,PA_OUTPUT_PA_BOOST_PIN); //for me this causes better signal strength but received corrupt messages
LoRa.setTxPower(TxPower, PA_OUTPUT_RFO_PIN);
LoRa.setSyncWord(SyncWord);
}

void loop() {

    while (Serial2.available() > 0)
    if (gps.encode(Serial2.read())){
        GenerateMessage();

        Serial.print("Sending packet: ");
        Serial.print(" ");
        Serial.println(counter);

        LoRa.beginPacket();
        LoRa.println(LocationMessage);
        LoRa.print(counter);
        LoRa.endPacket();

        counter += 1;

    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring or serial definition."));
    while(true);
    
  }



  

  
  counter++;

  delay(5000); 
}



void GenerateMessage()
{
  
  memset(LocationMessage, 0, sizeof(LocationMessage));

  Serial.print(F("Location: ")); 


  char Lat[10],Lng[10] ;


  if (gps.location.isValid())
  {

  dtostrf(gps.location.lat(),4,6,Lat);
  dtostrf(gps.location.lng(),4,6,Lng);
  strcat(LocationMessage,Lat);
  strcat(LocationMessage,",");
  strcat(LocationMessage,Lng); 
    Serial.print(gps.location.lat());
    Serial.print(F(","));
    Serial.print(gps.location.lng());
  }
  else
  {
    strcat(LocationMessage,"INVALID"); 
    Serial.print(F("INVALID - try to go out"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  
  Serial.println();
  Serial.print("Message");
  Serial.println(LocationMessage);  
  delay(TransmitInterval*1000);
}


void UBLOX_GPS_Shutdown()
{
  //sends command over serial interface to GPS to put it in PMREQ backup mode
  Serial.println("GPS sleep mode");
  uint8_t index;
  uint8_t UBLOX_GPSStandby[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B}; 

  for (index = 0; index < sizeof(UBLOX_GPSStandby); index++)
  {
    Serial2.write(UBLOX_GPSStandby[index]);
    Serial.print(UBLOX_GPSStandby[index], HEX);
    Serial.print(" ");
  }
}


void UBLOX_GPS_Wakeup()
{
  Serial2.println();                                   //send some characters to GPS to wake it up
  Serial.println("GPS wake up");
}