# Chassis Controller Hardware

## RS232 Serial Connection
Useful for getting debug output from the microcontroller. Uses a USB-to-RS232 adaptor and a custom DB9 to DTM-3P cable.

### Pinout

DB9 Connector

| Pin Number | Designation |
| --- | --- |
| 1 | |
| 2 | RX |
| 3 | TX |
| 4 | |
| 5 | Ground |
| 6 | |
| 7 | |
| 8 | |
| 9 | |

DTM-3P Connector

| Pin Number | Designation |
| --- | --- |
| 1 | Ground |
| 2 | TX |
| 3 | RX |

Remember that for serial to work RX and TX must be swapped or "crossed over" during the wiring.

## RS232 RF Modem

The XBee-PRO PKG-R® modules we have are a really simple way to make the debug connection to the chassis controller wireless. If working correctly they should be plug-n-play, so they're very easy to use.

The advertised range is around 12000m which should be enough to receive data from any point on a regular FSAE track. For example, lakeside raceway is around 250m from the furthest point on their small test track to the pits and the main track is around 600m to the furthest point to its respective pits.

The datasheet for this device is located in the datasheets folder of this repo.
