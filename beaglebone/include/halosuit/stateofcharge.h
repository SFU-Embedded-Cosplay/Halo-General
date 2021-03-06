/*
    stateofcharge.h
*/

#ifndef STATE_OF_CHARGE
#define STATE_OF_CHARGE

#define TURNIGY_8_AH    1
#define TURNIGY_2_AH    2
#define GLASS_BATTERY   3
#define PHONE_BATTERY   4

#define SAMPLE_SIZE 11
// if more data points are added increase the size of SAMPLE_SIZE 

// turnigy 8.4 Ah
typedef struct Battery {
    const int id;
    const int internal_resistance; // milliohms 
    int percent_charge;
    int adjusted_ocv;
    const int ocv[SAMPLE_SIZE];
} Battery;

void soc_init();

int soc_getcharge(int batteryID);

// can only set phone and glass battery
void soc_setcharge(int batteryID, int percent_charge);

#endif
