/*
 * ControllerFactory.c
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "EspWii.h"
#include "AbstractController.h"
#include "UdpController.h"
#include <Preferences.h>
#include <controller/LocalController.h>
#include <device/UdpNameSearch.h>
#include <wifi/WifiManager.h>
enum ControllerManager {
	Boot,
	WaitForClients,
	BeginSearch,
	WaitForSearchToFinish,
	ShutdownSearch,
	connectControllers,
	updateControllers
};
static String * controllerNamePointer = NULL;
static UDPSimplePacketComs * coms;
static Preferences preferences;
static String wifiSSID, wifiPassword;
static boolean useClient = true;
static IPAddress broadcast;
static ControllerManager state = Boot;
static long searchStartTime = 0;
static UdpNameSearch * pinger = NULL;
static std::vector<IPAddress*> * FactoryAvailibleIPs;
static std::vector<UdpController*> controllerList;
//when wifi connects
static Accessory localController;
static LocalController * local=NULL;
static WifiManager manager;


/**
 * Public functions
 */
void launchControllerReciver(String * controllerName) {
	controllerNamePointer=controllerName;
	state=Boot;

}


void loopReciver() {
	manager.loop();

	switch (state) {
	case Boot:
		state = WaitForClients;
		manager.setup();
		break;
	case WaitForClients:
		if (manager.getState()==Connected) {
			state = BeginSearch;
			if (!useClient)
				broadcast = WiFi.softAPIP();
			else{
				broadcast=WiFi.localIP();
			}
			broadcast[3] = 255;
			if(pinger==NULL){
				Serial.println("Pinnger built");
				coms = new UDPSimplePacketComs(&broadcast,useClient);
				pinger = new UdpNameSearch(coms,controllerNamePointer);
			}else{
				coms->targetDevice->fromString(broadcast.toString());
			}

		}
		//Serial.println("WaitForClients");
		break;
	case BeginSearch:
		searchStartTime = millis();
		pinger->oneShotMode();
		state = WaitForSearchToFinish;
		Serial.println("BeginSearch controller factory, Expect Timeouts "+coms->targetDevice->toString());
		break;
	case WaitForSearchToFinish:
		pinger->loop();
		if ((millis() - searchStartTime) > 5000) {
			state = ShutdownSearch;
		}
		//Serial.println("WaitForSearchToFinish");
		break;
	case ShutdownSearch:
		FactoryAvailibleIPs = getAvailibleIPs();
		if(FactoryAvailibleIPs->size()>0)
			state = connectControllers;
		else
			state = BeginSearch;
		Serial.println("ShutdownSearch controller factory");
		break;
	case connectControllers:
		if(FactoryAvailibleIPs->size()>0){
			Serial.println("Search Finished, found : "+String(FactoryAvailibleIPs->size()));
			for (int i=0;i<FactoryAvailibleIPs->size();i++) {
				IPAddress* tmp =FactoryAvailibleIPs->at(i) ;
				UDPSimplePacketComs * tmpCom = new UDPSimplePacketComs(tmp,useClient);
				Serial.print("Com's built");
				UdpController *controller =new UdpController(tmpCom);
				controllerList.push_back(controller);
				Serial.print("\r\nController built on: ");
				Serial.print(tmp[0]);
			}

			state = updateControllers;
		}else{
			state = BeginSearch;
		}
		break;
	case updateControllers:
		//Serial.println("checkForFailures");
		for (int i=0;i<controllerList.size();i++) {
			controllerList.at(i)->loop();
		}

		break;
	}
	if(local!=NULL){
		local->loop();
	}

}
AbstractController * getController(int id) {
	if(id==0){
		if(local==NULL)
			local= new LocalController(&localController);
		return local;
	}
	for (int i=0;i<controllerList.size();i++) {
		if(controllerList.at(i)->getId()==id)
			return controllerList.at(i);
	}
	return NULL;
}
