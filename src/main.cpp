#include <Arduino.h>
#include <ESP32Servo.h>
#include "AdafruitIO_WiFi.h"
#define PIN 4
#define LIGH_PIN 2
#define IO_USERNAME  "XXXXXXXXXXXX"
#define IO_KEY       "XXXXXXXXXXXX"
#define FEED "CHANGEPCDEV"
#define FEED_STATUS "STATUS_PC"

const char* ssid = "XXXXXXXXXXXX";      //SSID da rede WIFI
const char* password =  "XXXXXXXXXXXX@";    //senha da rede wifi    //password

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, ssid, password);

// configura o tópico "fabiosouza_io/feeds/L1"
AdafruitIO_Feed *feedL1 = io.feed(FEED);
AdafruitIO_Feed *feedStatus = io.feed(FEED_STATUS);

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
byte state;
int currentStatus;

void setup() {
    // put your setup code here, to run once:
  Serial.begin(115200);
    // Aguarda serial monitor
  while(!Serial);
  pinMode(LIGH_PIN,OUTPUT);
  digitalWrite(LIGH_PIN,LOW);
  myservo.attach(PIN);  // attaches the servo on pin 9 to the servo object
  state = 1;
}

void pressButtom(){
  myservo.attach(PIN);
  Serial.print("pressButtom");
  myservo.write(150);
  delay(250);
  myservo.write(120);
  myservo.detach();
}

int nomalizeData(char* value){
  int charAsnumber = strtol(value,NULL,10);
  Serial.print("nomalizeData: ");
  Serial.print(charAsnumber);
  if(charAsnumber<=3){
    return 1;
  }
  return 4;
}

void handleL1(AdafruitIO_Data *data) {

  // Mensagem 
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());
  Serial.println(currentStatus);
  int valueASNumber = nomalizeData(data->value());
  if(valueASNumber != currentStatus){
    pressButtom();
    feedStatus->save(valueASNumber);
  }
}

void updateStatus(AdafruitIO_Data *data) {
  Serial.print("updateStatus ");
  Serial.print(data->feedName());
  Serial.println(data->value());
  currentStatus = nomalizeData(data->value());
}


void conectaBroker(){
  
  //mensagem inicial
  Serial.print("Conectando ao Adafruit IO");

  // chama função de conexão io.adafruit.com
  io.connect();

   // instancia um novo handler para recepção da mensagem do feed L1 
  feedL1->onMessage(handleL1);
  feedStatus->onMessage(updateStatus);


  // Aguarda conexação ser estabelecida
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  digitalWrite(LIGH_PIN,HIGH);
  // Conectado
  Serial.println();
  Serial.println(io.statusText());

  // certifique-se de que todos os feeds obtenham seus valores atuais imediatamente
  feedStatus->get();
  feedL1->get();
}

void loop() {
  //verifica se está conectado
  if(state == AIO_NET_DISCONNECTED | state == AIO_DISCONNECTED){
    Serial.println("Vai reconectar....");
    conectaBroker(); //função para conectar ao broker
  }
  state = io.run();
  delay(250);
}

