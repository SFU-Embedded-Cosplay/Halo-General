/*
    serializer.c
*/

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include <json/serializer.h>
#include <json.h>
#include <json-builder.h>
#include <beagleblue/beagleblue.h>
#include <halosuit/halosuit.h>
#include <halosuit/stateofcharge.h>
//#include <testcode/automationtestdata.h>
#include <halosuit/automation.h>
#include <halosuit/logger.h>

#define ON "on"
#define OFF "off"
#define AUTO "auto"

#define BUFFER_SIZE 1024

static char* sound_buffer = NULL;

static void get_warnings(json_value *object)
{
    json_value *warnings = json_object_new(0);
    
    char headtempwarning = automation_getHeadTempWarning();
    switch (headtempwarning) {
        case CRITICAL_HIGH_TEMP_WARNING :
            json_object_push(warnings, "critical high head temperature", 
                json_string_new("HEAD TEMPERATURE CRITICALLY HIGH")); 
            break;
        case HIGH_TEMP_WARNING :
            json_object_push(warnings, "high head temperature", 
                json_string_new("HEAD TEMPERATURE HIGH")); 
            break;
        case LOW_TEMP_WARNING :
            json_object_push(warnings, "low head temperature", 
                json_string_new("HEAD TEMPERATURE LOW")); 
            break;
        case CRITICAL_LOW_TEMP_WARNING :
            json_object_push(warnings, "critical low head temperature", 
                json_string_new("HEAD TEMPERATURE CRITICALLY LOW")); 
            break;
        case NOMINAL_TEMP:
            break;
        default:
           logger_log("ERROR: HEAD TEMPERATURE WARNING UNDEFINED");
    } 

    char bodytempwarning = automation_getBodyTempWarning();
    switch (bodytempwarning) {
        case CRITICAL_HIGH_TEMP_WARNING :
            json_object_push(warnings, "critical high body temperature", 
                json_string_new("BODY TEMPERATURE CRITICALLY HIGH")); 
            break;
        case HIGH_TEMP_WARNING :
            json_object_push(warnings, "high body temperature", 
                json_string_new("BODY TEMPERATURE HIGH")); 
            break;
        case LOW_TEMP_WARNING :
            json_object_push(warnings, "low body temperature", 
                json_string_new("BODY TEMPERATURE LOW")); 
            break;
        case CRITICAL_LOW_TEMP_WARNING :
            json_object_push(warnings, "critical low body temperature", 
                json_string_new("BODY TEMPERATURE CRITICALLY LOW")); 
            break;
        case NOMINAL_TEMP:
            break;
        default:
           logger_log("ERROR: BODY TEMPERATURE WARNING UNDEFINED");
    } 

    char watertempwarning = automation_getWaterTempWarning();
    switch (watertempwarning) {
        case HIGH_TEMP_WARNING :
            json_object_push(warnings, "high water temperature", 
                json_string_new("WATER TEMPERATURE HIGH")); 
            break;
        case LOW_TEMP_WARNING :
            json_object_push(warnings, "low water temperature", 
                json_string_new("WATER TEMPERATURE LOW")); 
            break;
        case NOMINAL_TEMP :
            break;
        default:
            logger_log("ERROR: WATER TEMPERATURE WARNING UNDEFINED");
    }

    char flowwarning = automation_getWaterFlowWarning();
    switch (flowwarning) {
        case LOW_FLOW :
            json_object_push(warnings, "low water flow", json_string_new("LOW WATER FLOW, POSSIBLE LEAK"));
            break;
        case NOMINAL_FLOW :
            break;
        default:
            logger_log("ERROR: WATER FLOW WARNING UNDEFINED");
    } 

    char turnigy_8AH_warning = automation_getBatteryWarning(TURNIGY_8_AH);
    switch (turnigy_8AH_warning) {
        case LOW_SOC:
            json_object_push(warnings, "low 8AH battery warning", json_string_new("TURNIGY 8 AH LOW BATTERY"));
            break;
        case NOMINAL_SOC:
            break;
        default:
            logger_log("ERROR: TURNIGY 8AH BATTERY WARNING UNDEFINED");
    }

    char turnigy_2AH_warning = automation_getBatteryWarning(TURNIGY_2_AH);
    switch (turnigy_2AH_warning) {
        case LOW_SOC:
            json_object_push(warnings, "low 2AH battery warning", json_string_new("TURNIGY 2 AH LOW BATTERY"));
            break;
        case NOMINAL_SOC:
            break;
        default:
            logger_log("ERROR: TURNIGY 2AH BATTERY WARNING UNDEFINED");
    }

    char glass_battery_warning = automation_getBatteryWarning(GLASS_BATTERY);
    switch (glass_battery_warning) {
        case LOW_SOC:
            json_object_push(warnings, "low hud battery warning", json_string_new("GOOGLE GLASS LOW BATTERY"));
            break;
        case NOMINAL_SOC:
            break;
        default:
            logger_log("ERROR: GLASS BATTERY WARNING UNDEFINED");
    }

    char phone_battery_warning = automation_getBatteryWarning(PHONE_BATTERY);
    switch (phone_battery_warning) {
        case LOW_SOC:
            json_object_push(warnings, "low phone battery warning", json_string_new("PHONE LOW BATTERY"));
            break;
        case NOMINAL_SOC:
            break;
        default:
            logger_log("ERROR: PHONE BATTERY WARNING UNDEFINED");
    }
    json_object_push(object, "warnings", warnings); 
}



static void serializer_buildjson(json_value *object)
{
    int value = 0;
    // lights
    if (halosuit_relay_value(LIGHTS, &value) != 0) {
	    logger_log("ERROR: LIGHTS READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "lights", json_string_new(ON));
    }
    // auto lights
    else if (halosuit_relay_value(LIGHTS_AUTO, &value) != 0) {
	    logger_log("ERROR: LIGHTS_AUTO READ VALUE FAILURE\n");
    }	
    else if (value == 1) {
	    json_object_push(object, "lights", json_string_new(AUTO));
    }
    else {
	    json_object_push(object, "lights", json_string_new(OFF));
    }
    
    
    // head lights white
    if (halosuit_relay_value(HEADLIGHTS_WHITE, &value) != 0) {
	    logger_log("ERROR: HEADLIGHTS_WHITE READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "head lights white", json_string_new(ON));
    }
    else if (value == 0) {
	    json_object_push(object, "head lights white", json_string_new(OFF));
    }

    // head lights red
    if (halosuit_relay_value(HEADLIGHTS_RED, &value) != 0) {
	    logger_log("ERROR: HEADLIGHTS RED READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "head lights red", json_string_new(ON));
    }
    else if (value == 0) {
	    json_object_push(object, "head lights red", json_string_new(OFF));
    }
    
    // head fans
    if (halosuit_relay_value(HEAD_FANS, &value) != 0) {
	    logger_log("ERROR: HEAD_FANS READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "head fans", json_string_new(ON));
    }
    else if (value == 0) {
	    json_object_push(object, "head fans", json_string_new(OFF));
    }
    
    // water pump
    if (halosuit_relay_value(WATER_PUMP, &value) != 0) {
	    logger_log("ERROR: WATER_PUMP READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "water pump", json_string_new(ON));
    }
    else if (value == 0) {
	    json_object_push(object, "water pump", json_string_new(OFF));
    }

    // peltier
    if (halosuit_relay_value(PELTIER, &value) != 0) {
	    logger_log("ERROR: PELTIER READ VALUE FAILURE\n");
    }	
    if (value == 1) {
	    json_object_push(object, "peltier", json_string_new(ON));
    }
    else if (value == 0) {
	    json_object_push(object, "peltier", json_string_new(OFF));
    }

    // temperature readings 
    double temperature = 0;
    
    // head temperature
    if (halosuit_temperature_value(HEAD, &temperature) != 0) {
	    logger_log("ERROR: HEAD TEMPERATURE READ FAILURE\n");
    }
    else {
        json_object_push(object, "head temperature", json_double_new(temperature)); 
    }

    // armpits temperature 
    if (halosuit_temperature_value(ARMPITS, &temperature) != 0) {
	    logger_log("ERROR: ARMPITS TEMPERATURE READ FAILURE\n");
    }
    else {
        json_object_push(object, "armpits temperature", json_double_new(temperature));
    }

    // crotch temperature 
    if (halosuit_temperature_value(CROTCH, &temperature) != 0) {
	    logger_log("ERROR: CROTCH TEMPERATURE READ FAILURE\n");
    }
    else {
        json_object_push(object, "crotch temperature", json_double_new(temperature));
    }

    // water temperature 
    if (halosuit_temperature_value(WATER, &temperature) != 0) {
	    logger_log("ERROR: WATER TEMPERATURE READ FAILURE\n");
    }
    else {
        json_object_push(object, "water temperature", json_double_new(temperature));
    }

    // water flow
    int flow;
    if (halosuit_flowrate(&flow)) {
        logger_log("ERROR: FLOW RATE READ FAILURE\n");
    }
    else {
        json_object_push(object, "flow rate", json_integer_new(flow));
    }

    // heartrate
    int heartrate;
    if (halosuit_heartrate(&heartrate)) {
        logger_log("ERROR: HEART RATE READ FAILURE\n");
    }
    else {
        json_object_push(object, "heart rate", json_integer_new(heartrate));
    }

    json_object_push(object, "play sound", json_string_new(sound_buffer));

    
    // state of charge
    json_object_push(object, "8 AH battery", json_integer_new(soc_getcharge(TURNIGY_8_AH)));
    json_object_push(object, "2 AH battery", json_integer_new(soc_getcharge(TURNIGY_2_AH)));
    json_object_push(object, "hud battery", json_integer_new(soc_getcharge(GLASS_BATTERY)));
    json_object_push(object, "phone battery", json_integer_new(soc_getcharge(PHONE_BATTERY)));

    // Warnings 
    get_warnings(object);
    
} 
void serializer_serialize(char *buf)
{ 
    json_value *object = json_object_new(0);

    // push suit status values into object
    serializer_buildjson(object);

    json_serialize(buf, object);
   // logger_log(buf);
    json_builder_free(object);
}

void serializer_save_sound(char *buf)
{
    if (sound_buffer == NULL) {
        sound_buffer = (char*) (BUFFER_SIZE * sizeof(char));
    }

    sound_buffer = buf;
}
