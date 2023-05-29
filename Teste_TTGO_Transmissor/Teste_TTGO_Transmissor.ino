//inclusão de bibliotecas
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

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

//MPU 6050
int minVal=0;
int maxVal=10;
double x, y, z;
Adafruit_MPU6050 mpu;

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

  doc["EixoX"] = x;
  doc["EixoY"] = y;
  doc["EixoZ"] = z;
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

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  
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

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int xAng = map(a.acceleration.x,minVal,maxVal,-90,90);
  int yAng = map(a.acceleration.y, minVal, maxVal, -90, 90);
  int zAng = map(a.acceleration.z, minVal, maxVal, -90, 90);
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  Serial.print("Rotation X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.print(y);
  Serial.print(", Z: ");
  Serial.print(z);
  
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
