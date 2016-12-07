#include "mmap.h"
#include <SPI.h>
#include "RF24.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <nRF24L01.h>
#include "SIM900.h"
#include "inetGSM.h"

#define CestoQTD 2 //Node ID
#define NUM_TESTES 10
#define NUM_TESTES_OK 7
#define TAM_FILA 4  
#define TAM_FILA_2 1

using namespace std;

// Dados GSM
InetGSM inet;
boolean started = false;
char n2[20];
int q_rua = 1, v_rua = 0;
int v_rua_ant = 0;
int enviou = 0;

int Cestos[CestoQTD] = {1,2};
byte pipeNum; //estava 2 //variable to hold which reading pipe sent data
bool radioNumber = 0;
RF24 radio(7,8);
const uint64_t rAddress[] = {0xB3B4B5B6CDF1, 0xB3B4B5B6ABLL, 0xB3B4B5B6F1LL}; //0xB3B4B5B6F1LL
const uint64_t wAddress[] = {0x7878787878LL};
bool role = 0;
char data_send[150];
char data_send1[45];
char data_send2[30];
char data_rcv1[45];
char data_rcv2[45];
char data_rcv[150];
char s_milimetros_g[5];
char s_chovendo_g[2];
char s_tem_agua_g[2];
char s_prioridade_g[2];
char s_sensor_ok[2];
char s_comun_ok[2];
char s_novo[2];
char s_id[2];
char teste[35] = "testeGSM";
char instruction[20];
//int id;
char instruction2[20];
int id2;
int vol;
int agua;
int menu = 0;
int menu2 = 0;
int next = 0;
int periodo;
int periodo_temp;
int queue[] = {2,4,1,3,6};
int ind_fila = 0;
int queue2[] = {2,5};
int ind_fila2 = 0;
int vez  =1;
int nivel_lixo;
int powder = 0;
char data[15];
char data_ws[10];
int contador = 0;
int contador_2 = 0;
int contador2 = 0;
int contador2_2 = 0;
int contador_data = 0;
int vol_ok = 0;
int agua_ok = 0;
int ind_ensor;

int comun_ok = 0;
int comun_ok_2 = 0;
int sensor_ok = 0;
int teste_vol[NUM_TESTES];
int teste_agua[NUM_TESTES];
int novo = 0;
struct mmapData* p_mmapData; 
char cesto[45] = "1 data";
int periodoDormir = 5;
bool cestoSleep = false;
int n;
int dado;
int automatic = 1;

//dados recebidos do carro
int vel;

float latitude;
float longitude;
char latitude_st[15];
char longitude_st[15];

int id = 1;
int pkt = 1;

int cont = 0;
char valor[200];
char st_GSM[2000];


// Variáveis Feedback
int risco;
int vel_limit;
int ledPin2 = 2;
int ledPin3 = 3;
int ledPin4 = 4;
int buzzer = 5;


/******************************************************************************************************/

void setup() {

  // Inicializando o serial
  Serial.begin(9600);

    
  // Inicializando pinos dos leds
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  // Inicializando pino buzzer
  pinMode(buzzer,OUTPUT);

  // Inicializando o GSM
  if (gsm.begin(9600))
  {
    Serial.println(F("\nstatus=READY"));
    started = true;
  }
  else Serial.println(F("\nstatus=IDLE"));
  
  // Começa a rodar a aplicação
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Inicializando o RF
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108); 
   
  // Abre canais de leitura e escrita para cada um dos Arduinos
   radio.openReadingPipe(0,rAddress[0]); //1
   radio.openWritingPipe(wAddress[0]);
  
  // Começa a ouvir o RF 
  radio.startListening();
}

/******************************************************************************************************/

void feedback(int risco, bool vel){
  //2 -> Verde
  //3 -> Amarelo
  //4 -> Vermelho
  //5 -> Buzzer


//BUZZER
  if((vel == 1) && (v_rua_ant == 0)){
    //Ligando o buzzer com uma frequencia de 1500 hz.
    //tone(buzzer,1500);   
    digitalWrite(buzzer, 1);
    delay(1000);
   
    //Desligando o buzzer.
   //noTone(buzzer); 
   digitalWrite(buzzer, 0);
   vel = 0;
   
  } /*else if (vel==0) {
    //Ligando o buzzer com uma frequencia de 1500 hz.
    tone(buzzer,1350);   
    delay(1000);
   
    //Desligando o buzzer.
   noTone(buzzer); 
  }*/
//

//LEDS
  if (risco == 0){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }
  else if(risco == 1){
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);    
  }
  else if(risco == 2){
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);    
  }
  else  if(risco == 3){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);    
  }
//
}


/******************************************************************************************************/

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

/******************************************************************************************************/

// Função que liga o módulo GSM
void powerUpOrDown()
{
  //Liga o GSM Shield
  Serial.print(F("Liga GSM..."));
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  delay(1000);
  digitalWrite(6, HIGH);
  delay(1000);
  Serial.println(F("OK!"));
  digitalWrite(6, LOW);
  delay(500);
}

/******************************************************************************************************/

// Função de enviar dados pelo GSM
int envia_GSM()
{
  char msg[600];
  int numdata;
  Serial.println("Inicio envia_GSM");
  if (inet.attachGPRS("claro.com.br", "claro", "claro"))
    Serial.println(F("status=Conectado..."));
  else Serial.println(F("status=Nao conectado !!"));
  Serial.println("Conexão OK");
  delay(100);
  //char valor[] = "nivel_lixo=75&milimetros=20&tem_agua=1&prioridade=5&chovendo=0&bueiro_nome=1&sensor_ok=1&comunicacao_ok=1";
  
  Serial.println("Valor OK");
  Serial.println(valor);
  numdata = inet.httpPOST("segcar.herokuapp.com", 80, "/message/", valor, msg, 600);

  getRealData(msg);
  Serial.println("Terminou o post");
  delay(5000);
  Serial.println("Terminou o delay");
  return numdata;
}

/******************************************************************************************************/

// Função pra cortar e ficar com o dado bonito
int cutting(char *dadofinal, char *text, char *a, char *b){
  int text_length, alen, blen;
  int i, apos, bpos, j;
  
  for(text_length = 0; text[text_length] != '\0'; text_length++);
  for(alen = 0; a[alen] != '\0'; alen++);
  for(blen = 0; b[blen] != '\0'; blen++);
  
  apos = match(text,text_length, a, alen);
  bpos = match(text,text_length, b, blen);
  
  if((apos != -1) && (bpos != -1)) {
    //Serial.print("oi");

    //
    if((text_length >= alen)&&(text_length >= blen)){
      //Serial.print("entrei");
      j = 0;
      for(i = apos + alen; i < bpos; i++){
        dadofinal[j++] = text[i];
        Serial.print(text[i]);
      }
      //dadofinal[j++] = '\0';
      //Serial.println();
    }
    return 0;
  }
  else {
    Serial.println("Not found.\n");
    return -1;
  }  
}

/******************************************************************************************************/

// Função pra combinar alguma coisa do GSM
int match(char *text,int text_length, char *pattern, int pattern_length) {
  int c, d, e, pos = -1;
  
  if (pattern_length > text_length) {
    return -1;
  } 
  for (c = 0; c <= text_length - pattern_length; c++) {
    pos = e = c;
    for (d = 0; d < pattern_length; d++) {
      if (pattern[d] == text[e]) {
        e++;
      }
      else {
        break;
      }
    }
    if (d == pattern_length) {
      return pos;
    }
  }
  return -1;
}

/******************************************************************************************************/

// Função pra separar o dado real recebido pelo GSM
void getRealData(char *temp_string){
  //Serial.println(temp_string);
  char inicio[50];
  char dadofinal[50];
  char fim[50];
  
  int ret = -1;
  sprintf(inicio, "{");
  sprintf(fim, "}");
  if(!cutting(dadofinal, temp_string,inicio, fim)){
    //Serial.println("Olha as coisas bonitas ae gente!");
  }
  else {
    Serial.println("fail :(");
  }
  Serial.println("Dado final: ");
  Serial.println(dadofinal);
  sscanf(dadofinal, "\"q_rua\": %d, \"v_rua\": %d", &q_rua, &v_rua);
  Serial.println(q_rua);
  Serial.println(v_rua);
  
  
}

/******************************************************************************************************/

// Função que recebe os dados pelo GSM
void recebe_GSM()
{
  char temp_string[600];
  int ret;
  
  int numdata;
  //char temp_string[600];
  if (inet.attachGPRS("claro.com.br", "claro", "claro"))
    Serial.println(F("status=Conectado..."));
  else Serial.println(F("status=Nao conectado !!"));
  numdata = inet.httpGET("segcar.herokuapp.com", 80, "/carro/1/", temp_string, 600);
  if(numdata > 0 ){
      
    Serial.println("\nData received:"); 
    Serial.println(temp_string);
   }
   else {
    Serial.println("\nNumber of data received:");
    Serial.println(numdata);
   }
}

/******************************************************************************************************/

void loop()
{
    int numdata;
    Serial.println("loop");
    Serial.println(gsm.getStatus());
    
    while(radio.available(&pipeNum)){
      
      // Lê dado recebido do Arduino
      radio.read( &data_rcv, sizeof(data_rcv));
      Serial.print("Recebeu: ");
      Serial.println(data_rcv);
      sscanf(data_rcv, "%d %s %s", &vel, latitude_st, longitude_st);

      // Coloca dado recebido na string que vai mandar para o servidor
      sprintf(valor, "{\"id_carro\":%d,\"pkt\":%d,\"gps\":\"%s,%s\",\"vel\":%d}", id, pkt, latitude_st, longitude_st, vel);
      
      
      if ((pkt%10) == 1) {
        strcpy(st_GSM, "[");
        strcat(st_GSM, valor);
      }
      else if ((pkt%(10)) == 0) {
        strcat(st_GSM, ",");
        strcat(st_GSM, valor);
        strcat(st_GSM, "]");
      } else {
        strcat(st_GSM, ",");
        strcat(st_GSM, valor);
      }

      Serial.print("st_GSM: ");
      Serial.println(st_GSM);

      //strcpy(data_send, data_rcv);
      // Manda de volta o ack
      sprintf(data_send, "%d %d", q_rua, v_rua);
      sendRF();
      pkt++;
      cont++;
    }
    if (cont == 10) {
      envia_GSM();
      cont = 0;
    }
    delay(100);
   
    feedback(q_rua, v_rua);
    v_rua_ant = v_rua;
}

 // Loop
