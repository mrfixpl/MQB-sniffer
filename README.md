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

### Example data sniffed from OBDeleven-Car communication ###

## Request headers and modules ##
* `714` - instryment cluster (module 0x17)
* `7E1` - gearbox (module 0x02)

## Warnings ##
* Cheap ELM327 adapters will not work. Buffer will overflow with date before you can read the data from it.

## Shopping list ##
* OBDeleven adapter
* Carista adapter
* Arduino Nano
* MCP2515
* OBD2 splitter
* WS2818 LED panel
