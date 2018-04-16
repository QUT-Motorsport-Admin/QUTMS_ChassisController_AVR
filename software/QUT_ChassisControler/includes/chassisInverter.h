typedef struct Inverter
{
	uint8_t ID;
	uint16_t RPM;
	uint16_t temperature;
	uint16_t current;
	uint16_t duty;
}Inverter;

struct Inverter inverters[NUM_INVERTERS];