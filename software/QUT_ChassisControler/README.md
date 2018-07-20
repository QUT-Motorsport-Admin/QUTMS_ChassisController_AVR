# QUT Chassis Controller

This file will cover the spec & implimentation for the new chassis controller software. This version of the software is V2, taking over the older software written by Julius in 12/2016.

## Installing

You need a fair few tools in order to run and compile this program.

    gcc
    avr-gcc
    avrdude

If you are on a windows machine, you can install these and run them though Cygwin, as well as WinAVR. You can find more info on this, this is taught in the unit CAB202, so check blackboard. May be able to make this with the windows equivalent of Homebrew with Chocolatey, your results may vary though!

In your on a Mac machine, simply install Homebrew/Brew. Then run the command. Then revel in the superior development environment that is mac!

    brew install gcc avr-gcc avrdude

If your on linux, then you can use what ever package installer you want. More or less, if your running linux you know how to install packages.

## Compiling

Inside of the terminal window of your choice, simply run

    make all

This will use the MakeFile within this directory, and compile everything with the correct settings for the Chassis Controller.

You will need to slightly edit the MakeFile in order to indclude and exclude the files to compile with.

## Programming

Inside of the terminal window of your choice, simply run

    ./programController

This will execute the right command to use AVRDUDE to load the main.hex file. NOTE: this is only currently set up to use and Atmel-ICE programmer.

The one available from S9 (when they have it in stock), is an AVRISP MKII. Thus, if you are using a different programmer, please refer to the avrdude documentation.

If your on a mac. Due to Apple being a bit special, Mac OSX High Serria and onwards hates the Atmel-ICE programmer. You will likely need to follow this tutorial to make it work: https://medium.com/@davidramsay/avrdude-and-atmel-ice-on-osx-10-3-high-sierra-a0185b4e280a

This can be quite a dangerous install as you are loading an extension into your kernel. Perform at your own risk.

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