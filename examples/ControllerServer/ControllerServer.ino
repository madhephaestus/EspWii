#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <Esp32SimplePacketComs.h>
#include <EspWii.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <PacketEvent.h>

#define CONTROLLER_ID 2
#define TEAMNUMBER    22
String * controllerName = new String("GameController_" + String(TEAMNUMBER));

Accessory classic;
long timeSincePrint = 0;
uint8_t valuesLastSent[WII_VALUES_ARRAY_SIZE];
void setup() {
	launchControllerServer();
	//classic.enableEncryption(true);

	PacketEventAbstract *ptr = new WiiClassicServerEvent(&classic,
			CONTROLLER_ID);
	addServer(ptr);
	setNameUdpDevice(controllerName);

}

void loop() {
	loopServer();
	if (millis() - timeSincePrint > 20) {
		timeSincePrint = millis();
		classic.readData();

		//classic.printInputs();

		//classic.printInputs();
		//Serial.print("\r\n took = "+String(millis()-timeSincePrint)+" Values= ");
		bool print = false;
		for (int i = 0; i < WII_VALUES_ARRAY_SIZE; i++) {
			if (valuesLastSent[i]!= classic.values[i]) {
				print=true;
			}
		}
		if (print) {
			Serial.print("\r\n Values= ");
			for (int i = 0; i < 60; i++) {
				//Serial.print(" , "+String( getControllerStatus()[i]));
			}
			for (int i = 0; i < WII_VALUES_ARRAY_SIZE; i++) {
				Serial.print(" , " + String((uint8_t) classic.values[i]));
				valuesLastSent[i]= classic.values[i];
			}
		}
	}

}
