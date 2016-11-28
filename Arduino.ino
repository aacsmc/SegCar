#include <Wire.h>
#include <OBD.h>

#define TAM_ARRAY 10

COBD obd; /* for Model A (UART version) */

void setup()
{
  // we'll use the debug LED as output
  Serial.begin(38400);
  // start communication with OBD-II adapter
  obd.begin();
  // initiate OBD-II connection until success
  while (!obd.init());
}

void loop()
{
  int i, veloc[TAM_ARRAY], local;

  for(i=0, i < TAM_ARRAY, i++){
    delay(100);
    // save SPEED in variable 'value', return true on success
    if (obd.readPID(PID_SPEED, value)) {
      veloc[i] = value;
    }
  }

  // MÓDULO GPS PEGA OS DADOS

  // FORMAÇÃO DA STRING PARA ENVIAR PRA GALILEO

  // ENVIO PARA GALILEO

}