#include <SPI.h>
#include "RF24.h"
#include <nRF24L01.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Wire.h>
#include <OBD.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"

#define TAM_ARRAY 10
#define VEL_LIM 25 // Valor recebido do banco de dados

COBD obd; /* for Model A (UART version) */

// Variáveis radio
bool radioNumber = 1;
//1 - Arduino;
//0 - Galileo;
RF24 radio(7,8);

// Feedback
const int buzzer = 9; //buzzer to arduino pin 9

// GPS
TinyGPSPlus gps;
SoftwareSerial gps_serial(10, 11); //RX=pin 10, TX= pin 11

// Rede RF
const uint64_t wAddress[] = {0x7878787878LL, 0xB3B4B5B6CDF1, 0xB3B4B5B6CDLL, 0xB3B4B5B6F1LL};

// Dados comunicação
char data_send[45];
char data_rcv[45];

//Funcao de escrever no RF
void sendRF(){
     radio.stopListening();
     if (!radio.write( data_send, sizeof(data_send) )){
       Serial.println(F("failed writing"));
     }
    Serial.println(F("Mandando: "));
    Serial.println(data_send);
    Serial.println();
  
    radio.startListening();
}

//Funcao de ler do RF
boolean receiveRF(){
  
    unsigned long started_waiting_at = micros();
    boolean timeout = false;                                   
     
    while ( ! radio.available() ){
        if (micros() - started_waiting_at > 2000000 ){
            timeout = true;
            break;
        }      
    }
    if ( timeout ){
        Serial.println(F("Failed receiveing1, response timed out."));
    }else{
        radio.read( data_rcv, sizeof(data_rcv)  );
        Serial.println(F("Olha o que chegou1: "));
        Serial.println(data_rcv);
        Serial.println();
    }
    return timeout;
}

void setup()
{
  Serial.begin(38400);
  gps_serial.begin(9600);

  // start communication with OBD-II adapter
  obd.begin();
  // initiate OBD-II connection until success
  while (!obd.init());

  // Inicializa o pino buzzer
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  //Inicializa o RF
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108); 
    // Abre um canal de leitura e um de escrita para o RF
  radio.openWritingPipe(wAddress[1]);
  radio.openReadingPipe(1,wAddress[0]);
  // Começa a ouvir pelo RF
  radio.startListening();
}

void loop()
{
  int i, soma = 0, velocidade;

  // PEGAR DADOS DO OBD-II
  for(i=0, i < TAM_ARRAY, i++){
    delay(100);
    // save SPEED in variable 'value', return true on success
    if (obd.readPID(PID_SPEED, value)) {
      soma = soma + value;
                                                                                  Serial.print("Velocidade instantanea: ");
                                                                                  Serial.println(value);
    }

    if(value > VEL_LIM) {
        tone(buzzer, 1000); // Send 1KHz sound signal...
      } else {
        noTone(buzzer);     // Stop sound...
      }
  }
  velocidade = (int) soma/TAM_ARRAY;

  // MÓDULO GPS PEGA OS DADOS
  do {
    if (gps.encode(gps_serial.read())) {
      Serial.println("Quantidade de Satelites:");
      Serial.println(gps.satellites.value());
      Serial.println("Latitude:");
      Serial.println(gps.location.lat(), 6);
      Serial.println("Longitude:");
      Serial.println(gps.location.lng(), 6);
      Serial.println();
    }
  } while(!gps_serial.available());

                                                                                  Serial.print("Velocidade média: ");
                                                                                  Serial.println(velocidade);

  // FORMAÇÃO DA STRING PARA ENVIAR PRA GALILEO

  // ENVIO PARA GALILEO
  //sprintf(data_send, "&lat=%s&lon=%s&vel=%d", lat, lon, velocidade);
  //sendRF();
 
  // Fica esperando mensagem da Galileo
 // boolean timeout;
  //timeout = receiveRF();
  
  /*if (!timeout){
      // Verifica a informação recebida
      sscanf(data_rcv, "%d %s", &velocidade_rcv, instruction); // Pega mensagem que chegou e separa no id e na instrução
  }*/

}