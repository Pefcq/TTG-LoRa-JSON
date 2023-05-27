//inclusão de bibliotecas
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

//Define a pinagem da tela OLED
#define oled_scl 15
#define oled_sda 4
#define oled_rst 16

//Pinagem do modulador do radio SX1276
#define LoRa_MOSI 27
#define LoRa_MISO 19
#define LoRa_SCK 5

// Pinagem do transmissor de radio SX1276
#define LoRa_nss 18
#define LoRa_dio1 26
#define LoRa_nrst 14

//Dados da mensagem
int counter = 0;
String str;

//Serial Json
const int capacity = JSON_OBJECT_SIZE(5);
StaticJsonDocument<capacity> doc;

// Define o radio
SX1276 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst);

//Define o OLED
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, oled_scl, oled_sda, oled_rst);

//Protótipo da função de enviar
void sendPacket();

/*
    Nome da função: sendPacket
    objetivo: envia a Mensagem via LoRa.
*/
void sendPacket()
{
  int eixoX = random(-360,360);
  int eixoY = random(-360,360);
  int eixoZ = random(-360,360);
  int trigger = 0;

  counter++;
  if(counter > 600){
    trigger = 1;
  }

  doc["EixoX"] = eixoX;
  doc["EixoY"] = eixoY;
  doc["EixoZ"] = eixoZ;
  doc["Alarme"] = trigger;
  doc["Packet"] = counter;

  String msg;

  serializeJsonPretty(doc, msg);

  int statusr = radio.transmit(msg);
  Serial.println(msg);
}

/******************* função principal (setup) *********************/
void setup()
{
  Serial.begin(115200);

  //Inicia a modulação da antena
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);
  u8g2.begin();
  radio.begin();
  radio.setFrequency(434);
  
  //pinMode(LED,OUTPUT); //inicializa o LED

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 24, "Iniciado");
    u8g2.drawStr(0, 47,"com Sucesso!");
  } while (u8g2.nextPage());
  
  delay(1000);
}

/******************* função em loop (loop) *********************/
void loop()
{ 
  Serial.println("Enviando");
  Serial.print(counter);
  Serial.println("");

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 24, "Enviando dados:");
    u8g2.setCursor(0,47);
    u8g2.print(counter);
  } while (u8g2.nextPage());


  sendPacket(); //Envia Mssg
}
