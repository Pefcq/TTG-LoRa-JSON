//inclusão de bibliotecas
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <U8g2lib.h>

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

#define BAND    915E6  //Escolha a frequência

// Define o radio
SX1276 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst);

//Define o OLED
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/oled_scl, /* data=*/oled_sda, /* reset=*/oled_rst);

//Conexao InfoData
int rssi = 0;
float snr = 0;
float dataRata = 0;
String packSize = "--";

//Troca tela
int countSwap = 0;
int ind = 0;
char charSel[] = {'a', 'b', 'c'};
char swap = 'a';

/* Protótipo da função */
void LoRaDataRSSI();
void LoRaDataSNR();
void LoRaDataRate();
void cbk(int packetSize);

/*
  Nome da função: LoRaDataRSSI
  objetivo: imprime a RSSI/Potencia do sinal e tamanho do pacote recebido no display.
*/
void LoRaDataRSSI(){

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(5, 18);
    u8g2.print("Recebendo " + String(packSize) + " bytes");
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.setCursor(5, 48);
    u8g2.print("RSSI " + String(rssi) + " dBm");
  } while (u8g2.nextPage());

  Serial.print("RSSI " + String(rssi) + " dBm");

}

/*
  Nome da função: LoRaDataSNR
  objetivo: imprime a SNR/Ruído do sinal e tamanho do pacote recebido no display.
*/

void LoRaDataSNR(){

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(5, 18);
    u8g2.print("Recebendo " + String(packSize) + " bytes");
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.setCursor(5, 48);
    u8g2.print("SNR " + String(snr) + " dB");
  } while (u8g2.nextPage());

  Serial.print("SNR " + String(snr) + " dB");
  
}

/*
  Nome da função: LoRaDataRate
  objetivo: imprime a Velocidade do sinal e tamanho do pacote recebido no display.
*/

void LoRaDataRate(){

  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(5, 18);
    u8g2.print("Recebendo " + String(packSize) + " bytes");
    u8g2.setCursor(5, 48);
    u8g2.print("Data Rate " + String(dataRata) + " kbps");
  } while (u8g2.nextPage());

  Serial.print("Data Rate " + String(dataRata) + " kbps");
  
}

/*
  Nome da função: cbk
  recebe como parâmetos um inteiros (packetSize)
  objetivo: recebe a temperatura via LoRa e armazena na variável packet.
*/
void cbk(int packetSize) {
  packSize = String(packetSize,DEC);
  rssi = radio.getRSSI();
  snr = radio.getSNR();
  dataRata = radio.getDataRate();


  //Logica para seleção da tela de informação
  countSwap++;
  if(countSwap > 4){
    countSwap = 0;
    ind++;
    if(ind > 2){
      ind = 0;
    }
  }

  swap = charSel[ind];

  //Seleciona a informação p/ o Display do OLED
  switch(swap){
    case 'a':
     LoRaDataRSSI();
     break;
    case 'b':
     LoRaDataSNR();
     break;
    case 'c':
     LoRaDataRate();
     break;
  }
  
}

/******************* função principal (setup) *********************/
void setup()
{nm
  u8g2.begin();
  
  Serial.begin(115200);
  
  u8g2.clear();
  
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.drawStr(0, 16, "Iniciado com Sucesso!");
    u8g2.drawStr(0, 42,"Aguardando dados...");
  } while (u8g2.nextPage());
    
  Serial.println("Iniciado com Sucesso!");
  Serial.println("Aguardando os dados...");
  delay(1000);
  
  radio.begin(); // Habilita o rádio LoRa para receber dados
}

/******************* função em loop (loop) *********************/
void loop()
{
  delay(1);
  String str;
  int state = radio.receive(str);
  int packetSize = radio.getPacketLength();
  if (state == RADIOLIB_ERR_NONE) { //Verifica se há dados chegando via LoRa
    cbk(packetSize);
    delay(1000);                // Espera 500 milissegundos
    Serial.println("Recebendo!");
  }
  //teste de timeout na conexão
  else if(state == RADIOLIB_ERR_RX_TIMEOUT){
    
    u8g2.firstPage();
    do
    {
      u8g2.setFont(u8g2_font_7x14_tf);
      u8g2.setCursor(5, 18);
      u8g2.print("Queda na conexao!");
      u8g2.setFont(u8g2_font_6x13_tf);
      u8g2.setCursor(5, 48);
      u8g2.print("Esperando resposta...");
    } while (u8g2.nextPage());
  
    Serial.println("Perda na conexão");
  }
  delay(10);
}
