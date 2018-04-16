//shutdown circuits
//seg 4 pin 63 dig 1 pinj0	//inertia switch
//seg 5 pin 64 dig 2 pinj1	//left front upright
//seg 6 pin 66 dig 3 pinj3	//brake overtravel
//seg 7 pin 68 dig 4 pinj5	//right front upright
//seg 8 pin 77 dig 5 pina1	//driver E-Stop

//digital inputs
//dig 6 pin 90	//driver switch

//digital outputs
//pin 25	//driver lamp a
//pin 24	//driver lamp b

#define STOP_INERTIA_SWITCH			PINJ&(1<<PINJ0)
#define STOP_LEFT_FRONT_UPRIGHT		PINJ&(1<<PINJ1)
#define STOP_BRAKE_OVERTRAVEL		PINJ&(1<<PINJ3)
#define STOP_RIGHT_FRONT_UPRIGHT	PINJ&(1<<PINJ5)
#define STOP_DRIVER_ESTOP			PINA&(1<<PINA1)

#define SHDN_INERTIA_SWITCH			1
#define SHDN_LEFT_FRONT_UPRIGHT		2
#define SHDN_BRAKE_OVERTRAVEL		4
#define SHDN_RIGHT_FRONT_UPRIGHT	8
#define SHDN_DRIVER_ESTOP			16
#define IGNITION_PORT				PINJ
#define IGNITION_PIN				PJ6