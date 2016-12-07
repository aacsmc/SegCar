#include "TinyGPS++.h"
#include <Wire.h>
#include <OBD.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "RF24.h"
#include <nRF24L01.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

TinyGPSPlus gps;
COBD obd; 

// Variáveis radio
bool radioNumber = 1;
//1 - Arduino;
//0 - Galileo;
RF24 radio(7,8);
const uint64_t wAddress[] = {0x7878787878LL, 0xB3B4B5B6CDF1, 0xB3B4B5B6CDLL, 0xB3B4B5B6F1LL};

// Dados comunicação
char data_send[150];
char data_rcv[150];
char instruction[20];
char* str;

int i = 0;
int velocidade[] = {0,0,0,0,0,0,0,0,0,0};
int vel = 20;
double latitude = 0, longitude = 0;
char* sptr;
double lat_old;
double lng_old;

int cont = 0;

char lat_st[10];
char lon_st[10];

int j = 0; // Sincronizar gps com rf
int id = 2;
int pkt = 0;


 int risco;
 int vel_limit;



int LerVelocidade(){
  int value;

  if (obd.readPID(PID_SPEED, value)){
    return value;
  }
  return 0;
}

void ImprimirDados(int vel, double lat, double lng){
    Serial.print("Velocidade media = ");
    Serial.println(vel);
    Serial.print("Lat = ");
    Serial.print(lat, 6);
    Serial.print(" || Lng = ");
    Serial.println(lng, 6);
    Serial.println();
}

int Media(int vel[]){
    int soma = 0, x;

    for(x=0; x<10; x++){
      soma += vel[x];
    }
    return ((int) (soma/10));
}

char* CriarString(char* txt,int vel, double lat, double lng){
  char out1[11];
  char out2[12];

  if((lat == 0.0) && (lng == 0.0)) {
    dtostrf(lat_old,9, 6, out1);
    dtostrf(lng_old,10, 6, out2);
  } else {
    dtostrf(lat,9, 6, out1);
    dtostrf(lng,10, 6, out2);

    lat_old = lat;
    lng_old  = lng;
  }
  sprintf(txt,"%d %s %s", vel, out1, out2);

  return txt;
}


void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);



  sptr = (char*) malloc(36*sizeof(char));

  //obd.begin();
  //while (!obd.init());

  // Inicializa os valores de latitude e longitude
  latitude = 0;
  longitude = 0;

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

void loop()
{
  // PEGAR A LOCALIZAÇÃO
  delay(40);
  while(Serial2.available()){
    if (gps.encode(Serial2.read())) {
      j = 1;
      //latitude = gps.location.lat();
      //longitude = gps.location.lng();
    }
  }
  
  // PEGAR A VELOCIDADE
  delay(40);

  if (cont <200) {
    vel = 20;

    // Rua daqui = Risco baixo
    // -8.055541, -34.951406
    latitude = -8.055541;
    longitude = -34.951406;
    
  }
  else if (cont>=200 && cont<500) {
    
    vel = 70;
    
    // Risco médio = avenida rui barbosa
    //-8.042629, -34.902495
    latitude = -8.042629;
    longitude = -34.902495;
    
  } else if (cont>= 500 && cont<1000) {
    vel = 20;
    
    // Risco alto = avenida boa viagem
    //-8.125814, -34.897388
    latitude = -8.125814;
    longitude = -34.897388;
  } else if (cont == 2000) cont = 0;
  
  velocidade[i] = vel;

  // IMPRIMIR OS DADOS PARA VISUALIZAÇÃO (SERÁ TROCADO PELO MÓDULO GSM)
  delay(20);
  if(i == 9){
    ImprimirDados(Media(velocidade),latitude,longitude);
    str = CriarString(sptr,Media(velocidade),latitude,longitude);

    if (j ==1) {
        // Arduino manda mensagem para Galileo
        strcpy(data_send, str);        
        sendRF();
       
        // Fica esperando mensagem da Galileo
        boolean timeout;
        timeout = receiveRF();

        sscanf(data_rcv, "%d %d", &risco, &vel_limit);
        delay(100);
        j = 0;
      }
      
    i = 0;
    latitude = 0;
    longitude = 0;
  } else{
      Serial.print("Velocidade = ");
      Serial.println(velocidade[i]);
      i++;
    }
    delay(100);
    cont++;

    
}
