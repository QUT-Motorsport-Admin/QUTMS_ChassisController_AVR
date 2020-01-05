![PCB Image](https://raw.githubusercontent.com/QUT-Motorsport/QUTMS_ChassisController/master/chassis_controller_pcb.png)

# Chassis Controller

The role of the chassis controller is to manage and coordinate all of the electrical systems on the car. At regular intervals it reads inputs and monitors devices on the car, checking they are responding to heartbeat messages within a specified time.

## Features

- 3 CANBUS interfaces using the MCP2515 controller
- 2 UART serial interfaces
- 14 analog inputs
- 9 digital inputs
- Dual 5A H-bridge throttle controller
- 60 pin MoTeC compatible connector

## System Architecture

### Serial Communication
The chassis controller has two UART interfaces, one which is dedicated to debugging. There is a [specification](https://docs.google.com/spreadsheets/d/1M0LFDXtBvufXvVOvwZmF34SnBRF86Ai6lYGNQQMeDdA/edit?usp=sharing "QUTMS_UART_Packet_Def") for a protocol to be used when the chassis controller is communicating with configuration software.


### Block Diagram

![RoughOutline](Chassis_Controller.png)
