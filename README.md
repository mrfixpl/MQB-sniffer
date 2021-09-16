# MQB-sniffer
CAN-BUS sniffing for Volkswagen Golf MK7 (and most likely other MQB platform cars).
Currently this is more a work log for future reference.

## Hardware used (not everything is required) ## 
* Car: VW Golf MK7, 2016, 2.0TDI (110kW, CRLB) + DSG (DQ250)
* OBDeleven (GEN2) OBD2 adapter + iOS device + OBDeleven account with PRO license
* ELM327 v1.5 (Carista)
* MCP2515 + Arduino Nano + bunch of cables + PC/Mac/Linux
* OBD2 cable splitter (to connect two OBD2 devices at the same time to the car)

## Sniffing ##
### Cables and adapters ###
* OBD2 Splitter connected to the car
* OBDeleven connected to one of the ports
* Carista ELM327 connected to the other port
* iOS device connected with OBDeleven adapter
* Mac/PC/Linux connected with Carsita ELM327 dongle
* serial monitor app running on Mac/PC, Carista ELM327 serial port connected at high baudrate (115200bps)

### ELM327 config ###
Commands to send to Carista ELM327 to prepare it for data sniffing:
* `at z` - reset ELM327 chip
* `at d` - defaults
* `at sp 6` - set CAN-BUS to 11bit, 500kbps
* `at cfc1` - flowcontrol
* `at d1` - show DLC
* `at al` - enable long messages
* `at h1` - show headers
* `at caf0` - disable formating
* `at l0` - disable linefeeds
* `at s1` - enable whitespaces (easier to view data)

### Data gathering ###
* `at ma` - start sniffing
* At the same time use OBDeleven to connect with car, go to list of modules, select  module, select live data, select data type, view.
* Carista ELM327 will sniff the communication between OBDeleven and car.

## Request headers and modules ##
* `714` - instrument cluster (module 0x17)
* `7E1` - gearbox (module 0x02)
* More to check: https://github.com/commaai/openpilot/issues/1238#issuecomment-602155568

## Example data sniffed from OBDeleven-Car communication ##
### Starting and maintaining communication with instrument cluster ###
`714 8 02 10 03 55 55 55 55 55` - Accessing instrument cluster<br>
`77E 8 06 50 03 00 32 00 C8 AA` - Cluster reply<br>
<br>
`714 8 03 19 02 AE 55 55 55 55`<br>
`77E 8 03 59 02 19 AA AA AA AA`<br>
<br>
`714 8 02 3E 00 55 55 55 55 55` - I'm here, don't sleep<br>
`77E 8 02 7E 00 AA AA AA AA AA` - not sleeping!<br>
<br>
`714 8 02 3E 00 55 55 55 55 55`<br>
`77E 8 02 7E 00 AA AA AA AA AA`<br>
`...`<br>

### RPM from instrument cluster ###
`714 8 03 22 22 D1 55 55 55 55` - request<br>
`77E 8 05 62 22 D1 1F 98 AA AA` - reply (0x1F98 = 8088 DEC; 8808 / 4 = 2022RPM)<br>

### Phototransistor instrument cluster (ambient light sensor) ###
`714 8 03 22 22 4D 55 55 55 55` - request<br>
`77E 8 04 62 22 4D FA AA AA AA` - reply (0xFA = 250 DEC; 250 out of 0-255 range; higher=brighter)<br>

### Enagaged gear from DSG ###
`7E1 8 03 22 38 16 55 55 55 55` - request<br>
`7E9 8 04 62 38 16 00 AA AA AA` - gear not engaged<br>
`7E9 8 04 62 38 16 0C AA AA AA` - reverse<br>
`7E9 8 04 62 38 16 02 AA AA AA` - 1st<br>

### Gearbox mode from DSG ###
`7E1 8 03 22 38 15 55 55 55 55` - request<br>
`7E9 8 04 62 38 15 00 AA AA AA` - P<br>
`7E9 8 04 62 38 15 01 AA AA AA` - R<br>
`7E9 8 04 62 38 15 02 AA AA AA` - N<br>
`7E9 8 04 62 38 15 03 AA AA AA` - D<br>
`7E9 8 04 62 38 15 04 AA AA AA` - S<br>
`7E9 8 04 62 38 15 05 AA AA AA` - M<br>

## Warnings ##
* Cheap ELM327 adapters will not work. Buffer will overflow with date before you can read the data from it.
* For some reasons, when trying to use MCP2515 to sniff, experiencing communication problems between OBDeleven and the car. Tried both with and without EOL termination resistor with no luck.

## Shopping list ##
* OBDeleven adapter
* Carista adapter
* Arduino Nano
* MCP2515
* OBD2 splitter
* WS2818 LED panel
