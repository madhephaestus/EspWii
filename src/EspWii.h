
#if !defined(ESPWIIH)
#define ESPWIIH
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <SimplePacketComs.h>
#include <Esp32SimplePacketComs.h>
#include <WiiChuck.h>
#include <server/UDPSimplePacket.h>
#include <server/WiiClassicServer.h>
//#include <server/WiiChuckServer.h>
#include <controller/UdpController.h>
#include "controller/AbstractController.h"
#include <PacketEvent.h>
//#include <controller/ControlFactory.h>

void launchControllerReciver(String * controllerName);
void loopReciver();

AbstractController * getController(int id);
#endif
