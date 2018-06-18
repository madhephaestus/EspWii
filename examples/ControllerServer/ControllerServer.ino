#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Esp32SimplePacketComs.h>
#include <PacketEvent.h>

#define CONTROLLER_ID 2
#define TEAMNUMBER    22
String * controllerName = new String("GameController_"+String(TEAMNUMBER));

Accessory  classic;
long timeSincePrint=0;
void setup() {
	launchControllerServer();
	PacketEventAbstract *ptr =new WiiClassicServerEvent(&classic,CONTROLLER_ID);
	addServer(ptr);
	setNameUdpDevice(controllerName);

}

void loop() {
	loopServer();
	if(millis()-timeSincePrint>50){
		timeSincePrint=millis();
		classic.readData();
		//classic.printInputs();

		//classic.printInputs();
		//Serial.print("\r\nValues=");
		for(int i=0;i<60;i++){
			//Serial.print(" , "+String( getControllerStatus()[i]));
		}
		for(int i=0;i<WII_VALUES_ARRAY_SIZE;i++){
			//Serial.print(" , "+String( (uint8_t)classic.values[i]));
		}

	}

}
