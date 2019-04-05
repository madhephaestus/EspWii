# EspWii
A framework for controlling one ESP from another using various Wii accessories. 

This library is for hosting an i2c Wii accessory device, supported by  [WiiChuck](https://github.com/madhephaestus/WiiChuck), as a UDP server. There is also a client in the library for connecting to that server. It lets you plug any Wii accessoryinto one ESP32, then use that accessory on a different ESP32 over the infrastructure WIFI. 

# Depenencies
In the Arduino Library Manager, search for and install:

```
WiiChuck
Esp32SimplePacketComs
Esp32WifiManager
SimplePacketComs
ESP32Servo
```

# Theory of opperation

This stack assumes you have 2 ESP32's where one is connected to a Wii accessory and the other is a robot to be controlled. 

For the game controller side, program the ControllerServer example and be sure to change the name of the game controller to something unique for your robot.

On the robot side, set the name to the same name as the one programmed into the game controller. Program the robot side. 

To connect to WiFi use the serial API provided in Esp32SimplePacketComs:

https://github.com/madhephaestus/Esp32SimplePacketComs/blob/master/README.md

### Get Name 
| |ID | bytes |
|--- |--- | --- |
| downstream  Bytes |4 | 60 |
| Contents downstream |1776 | String of Name or \* |
| upstream  Bytes |4 | 60 |
| Contents upstream |1776 | String of Name|

This checks the device to see if the name of the device matched a given name. If the name matches a packet is returned with the device name in its contents. If the name does not match, no packet is returned. 

If the name requests ends with a \* charrecter then it wil be treated as a wildcard. Matches up to the \* will be returned. Name requests with just \* will always return the name of the device. 

## Game Controller

| |ID | byte 0 |byte 1 |byte 2 |byte 3 |byte 4 |byte 5 |byte 6 |byte 7 |byte 8 |byte 9 |byte 10 |byte 11 |byte 12 |byte 13 |byte 14 |byte 15 |byte 16 |byte 17 |byte 18 |byte 19 |
|--- |--- | --- |--- | --- |--- | --- | --- | --- |--- | --- |--- | --- | --- | --- |--- | --- |--- | --- | --- |--- |--- |
| downstream Bytes |4 | 1 | 1 | 1 | 1 | 1 | 1 |1 | 1 | 1 | 1 | 1 | 1 |1 | 1 | 1 | 1 | 1 | 1 |1|1|
| Contents downstream |1970 | Controller ID| JoyXLeft | JoyYLeft | JoyXRight | JoyYRight | TriggerLeft | TriggerRight |PadRight/PadLeft| PadDown/PadUp |--- |--- |ButtonX|ButtonY|ButtonA|ButtonB|ButtonMinus/getButtonPlus|ButtonHome|---|ButtonZLeft|ButtonZRight|
| upstream Bytes |4 | 60 |
| Contents upstream |1970 | Byte data to be printed by the game controller server|

This command exchanges data with a game controller. WHen accessed remotely the ID is seperated out from the data. Data for a given axis starts at index 0 with JoyXLeft and ending with index 18 for ButtonZRight.
