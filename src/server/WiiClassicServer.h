/*
 * WiiClassicServer.h
 *
 *  Created on: Mar 5, 2018
 *      Author: hephaestus
 */

#ifndef ESWPIISERV
#define ESWPIISERV
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <PacketEvent.h>
#include <cmath>        // std::abs
#include <WiiChuck.h>

#define WII_CLASSIC_SERVER 1970
#include "ServerStatus.h"
class WiiClassicServerEvent: public PacketEventAbstract {
private:
	uint8_t controllerIndex;
	void start(){

		c->begin();
	}
public:
	Accessory * c;
	// Packet ID needs to be set
	WiiClassicServerEvent(Accessory * incomingClassic,uint8_t mycontrollerIndex) :
			PacketEventAbstract(WII_CLASSIC_SERVER) {
		c = incomingClassic;
		controllerIndex = mycontrollerIndex;
		start();

	}
	WiiClassicServerEvent(uint8_t mycontrollerIndex) :
			PacketEventAbstract(WII_CLASSIC_SERVER) {
		c = new Accessory();
		controllerIndex = mycontrollerIndex;
		start();
	}

	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer);
};

#endif /* ESWPIISERV */
