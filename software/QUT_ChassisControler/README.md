# QUT Chassis Controller

This file will cover the spec & implimentation for the new chassis controller software. This version of the software is V2, taking over the older software written by Julius in 12/2016.

## Conventions

### Naming

All of the functions that are/will be coded for the project should be `<module>_<name>`. This way all operations are inherently namespaced and wont cause conflict. It will also mean that code will be instantly more readable.

## Spec

### Processes

#### Main

#### StartUp

1. Init the whole car
2. Iteract with user to select car mode
3. Arm Car

#### CAN Heartbeat/IsAlive
This function should periodicly check to see the various state changes and most importantly, if the circuits are still alive.

* Every function loop
    * Check Motor Controllers / Inverters
    * 12 times: Check Power Distrubution unit
    * 24 times: Check Accumulators
    * 48 times: Check Steering Wheel

### Interrupts

### CANBUS
There are three CANBUS's. Each is defined as a level of importance/system. These bus's controllers are on the chassis controller, using the MCP2515 chip.

 1. CANBUS 1
    * Used: Motor inverter/controller bus
    * Importance: "High" priority
    * mHz: <speed>
 2. CANBUS 2
    * Used: Power Distrubution/Shutdown, Accumulator
    * Importance: "Medium" priority
    * mHz: <speed>
 3. CANBUS 3
    * Used: Communications (Radio), Steering Wheel
    * Importance: "Low" priority
    * mHz: <speed>