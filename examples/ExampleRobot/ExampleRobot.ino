/*

 Simple web configuration portal

 When it is connected to a WiFi network, everything is fine
 When connection fails, it create a portal to insert a new SSID and password,
 If the new credentials works, it will be stored in ESP32 non volatile storage (NVS).

 In a nutshell: it's a https://github.com/tzapu/WiFiManager for ESP32 with a non blocking implementation and simple design.

 Connect to wifi network generated by ESP32 and tip in your browser:
 AP IPv4: 192.168.4.1  or  AP IPv6

 by Evandro Luis Copercini - 2017
 Public Domain
 */

/*
 TODO: integrate with https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
 */

#include "WiFi.h"
#include <Preferences.h>

#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here

WiFiServer server(80);
Preferences preferences;
static volatile bool wifi_connected = false;
String wifiSSID, wifiPassword;
boolean useClient = false;
IPAddress broadcast;
void WiFiEvent(WiFiEvent_t event) {
	switch (event) {

	case SYSTEM_EVENT_AP_START:
		//can set ap hostname here
		WiFi.softAPsetHostname(AP_SSID);
		//enable ap ipv6 here
		WiFi.softAPenableIpV6();
		break;

	case SYSTEM_EVENT_STA_START:
		//set sta hostname here
		WiFi.setHostname(AP_SSID);
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6:
		//both interfaces get the same event
		Serial.print("STA IPv6: ");
		Serial.println(WiFi.localIPv6());
		Serial.print("AP IPv6: ");
		Serial.println(WiFi.softAPIPv6());
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		wifiOnConnect();
		wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		wifi_connected = false;
		wifiOnDisconnect();
		break;
	case SYSTEM_EVENT_WIFI_READY: /**< ESP32 WiFi ready */
		Serial.println("ESP32 WiFi ready ");
		break;
	case SYSTEM_EVENT_SCAN_DONE: /**< ESP32 finish scanning AP */
		Serial.println("SYSTEM_EVENT_SCAN_DONE");
		break;

	case SYSTEM_EVENT_STA_STOP: /**< ESP32 station stop */
		Serial.println("SYSTEM_EVENT_STA_STOP");
		break;

	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: /**< the auth mode of AP connected by ESP32 station changed */
		Serial.println("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
		break;
	case SYSTEM_EVENT_STA_LOST_IP: /**< ESP32 station lost IP and the IP is reset to 0 */
		Serial.println("SYSTEM_EVENT_STA_LOST_IP");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: /**< ESP32 station wps succeeds in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED: /**< ESP32 station wps fails in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_FAILED");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: /**< ESP32 station wps timeout in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_PIN: /**< ESP32 station wps pin code in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_PIN");
		break;

	case SYSTEM_EVENT_AP_STOP: /**< ESP32 soft-AP stop */
		Serial.println("SYSTEM_EVENT_AP_STOP");
		break;
	case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STACONNECTED");
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED: /**< a station disconnected from ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STADISCONNECTED");
		break;
	case SYSTEM_EVENT_AP_PROBEREQRECVED: /**< Receive probe request packet in soft-AP interface */
		Serial.println("SYSTEM_EVENT_AP_PROBEREQRECVED");
		break;

	case SYSTEM_EVENT_ETH_START: /**< ESP32 ethernet start */
		Serial.println("SYSTEM_EVENT_ETH_START");
		break;
	case SYSTEM_EVENT_ETH_STOP: /**< ESP32 ethernet stop */
		Serial.println("SYSTEM_EVENT_ETH_STOP");
		break;
	case SYSTEM_EVENT_ETH_CONNECTED: /**< ESP32 ethernet phy link up */
		Serial.println("SYSTEM_EVENT_ETH_CONNECTED");
		break;
	case SYSTEM_EVENT_ETH_DISCONNECTED: /**< ESP32 ethernet phy link down */
		Serial.println("SYSTEM_EVENT_ETH_DISCONNECTED");
		break;
	case SYSTEM_EVENT_ETH_GOT_IP: /**< ESP32 ethernet got IP from connected AP */
		Serial.println("SYSTEM_EVENT_ETH_GOT_IP");
		break;
	case SYSTEM_EVENT_MAX:
		Serial.println("SYSTEM_EVENT_MAX");
		break;
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("Waiting 10 seconds for WiFi to clear");
	WiFi.disconnect(true);
	delay(5000); // wait for WiFI stack to fully timeout
	Serial.println("Waiting 5 seconds for WiFi to clear");
	delay(5000); // wait for WiFI stack to fully timeout

	WiFi.onEvent(WiFiEvent);
	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.softAP(AP_SSID, AP_PW);
	Serial.println("AP Started");
	Serial.print("AP SSID: ");
	Serial.println(AP_SSID);
	Serial.print("AP IPv4: ");
	Serial.println(WiFi.softAPIP());
	broadcast = WiFi.softAPIP();
	broadcast[3] = 255;

	preferences.begin("wifi", false);
	wifiSSID = preferences.getString("ssid", "none");           //NVS key ssid
	wifiPassword = preferences.getString("password", "none"); //NVS key password
	preferences.end();
	if (wifiSSID != "none")
		useClient = true;
	if (useClient) {
		Serial.print("Stored SSID: ");
		Serial.println(wifiSSID);

		WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
	}
	server.begin();
}

void loop() {
	if (wifi_connected) {
		wifiConnectedLoop();
	} else {
		wifiDisconnectedLoop();
	}
}

//when wifi connects
void wifiOnConnect() {
	Serial.println("STA Connected");
	Serial.print("STA SSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("STA IPv4: ");
	Serial.println(WiFi.localIP());
	Serial.print("STA IPv6: ");
	Serial.println(WiFi.localIPv6());
	WiFi.mode(WIFI_MODE_STA);     //close AP network
}

//when wifi disconnects
void wifiOnDisconnect() {
	if (useClient) {
		Serial.println("STA Disconnected");
		delay(1000);
		WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
	}
}

//while wifi is connected
void wifiConnectedLoop() {
	Serial.print("RSSI: ");
	Serial.println(WiFi.RSSI());
	delay(1000);
}

void wifiDisconnectedLoop() {
	WiFiClient client = server.available();   // listen for incoming clients

	if (client) {                             // if you get a client,
		Serial.println("New client");     // print a message out the serial port
		String currentLine = ""; // make a String to hold incoming data from the client
		while (client.connected()) {        // loop while the client's connected
			if (client.available()) { // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);              // print it out the serial monitor
				if (c == '\n') {           // if the byte is a newline character

					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();

						// the content of the HTTP response follows the header:
						client.print(
								"<form method='get' action='a'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>");

						// The HTTP response ends with another blank line:
						client.println();
						// break out of the while loop:
						break;
					} else {    // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				} else if (c != '\r') { // if you got anything else but a carriage return character,
					currentLine += c;    // add it to the end of the currentLine
					continue;
				}

				if (currentLine.startsWith("GET /a?ssid=")) {
					//Expecting something like:
					//GET /a?ssid=blahhhh&pass=poooo
					Serial.println("");
					Serial.println("Cleaning old WiFi credentials from ESP32");
					// Remove all preferences under opened namespace
					preferences.clear();

					String qsid;
					qsid = currentLine.substring(12, currentLine.indexOf('&')); //parse ssid
					Serial.println(qsid);
					Serial.println("");
					String qpass;
					qpass = currentLine.substring(
							currentLine.lastIndexOf('=') + 1,
							currentLine.lastIndexOf(' ')); //parse password
					Serial.println(qpass);
					Serial.println("");

					preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
					Serial.println("Writing new ssid");
					preferences.putString("ssid", qsid);

					Serial.println("Writing new pass");
					preferences.putString("password", qpass);
					delay(300);
					preferences.end();

					client.println("HTTP/1.1 200 OK");
					client.println("Content-type:text/html");
					client.println();

					// the content of the HTTP response follows the header:
					client.print("<h1>OK! Restarting in 5 seconds...</h1>");
					client.println();
					Serial.println("Restarting in 5 seconds...");
					delay(5000);
					ESP.restart();
				}
			}
		}
		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
}