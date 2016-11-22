#include <Wire.h>
#include <OBD.h>

COBD obd; /* for Model A (UART version) */

void setup()
{
  // we'll use the debug LED as output
  Serial.begin(38400);
  pinMode(13, OUTPUT);
  // start communication with OBD-II adapter
  obd.begin();
  // initiate OBD-II connection until success
  while (!obd.init());
}

void loop()
{
  Serial.println("loop");
  int value;
  // save engine RPM in variable 'value', return true on success
  delay(100);
  if (obd.readPID(PID_SPEED, value)) {
    Serial.print("Speed = ");
    Serial.println(value);
  }
  //delay(1000);
 /* if (obd.readPID(PID_RPM, value)) {
    Serial.print("RPM = ");
    Serial.println(value);
  }*/
}
