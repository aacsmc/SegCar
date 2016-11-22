#include <OBD2UART.h>

COBD obd;

void setup(){
	Serial.begin(38400);
	obd.begin();
	while (!obd.init()){
		Serial.println("INIT FAIL!");
		Serial.println(obd.getState());
	}
	Serial.println(obd.getState());
	obd.setProtocol(PROTO_KWP2000_FAST);
	Serial.println(obd.getState());
}

void loop(){
	int value ;
 /* while (!obd.init()){
    Serial.println("INIT FAIL!");
    Serial.println(obd.getState());
  }*/
	Serial.println(obd.getState());
	obd.readPID(PID_RPM, value);
	Serial.print("RPM: ");
	Serial.println(value);
  delay(1000);
  obd.readPID(PID_SPEED, value);
  Serial.print("Velocidade: ");
  Serial.println(value);
	Serial.println(obd.getState());
 //obd.end();
}
