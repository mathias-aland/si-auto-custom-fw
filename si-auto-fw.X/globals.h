/* 
 * File:   globals.h
 * Author: Mathias
 *
 * Created on den 2 mars 2025, 15:29
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// must be 15 bytes
typedef struct
{
    uint16_t set_time;
    uint16_t degas_time;    
    uint8_t set_temp;
    uint8_t autodrain_time;
    uint8_t degas_pulse_off;
    uint8_t degas_pulse_on;
    uint8_t unused1;
    uint8_t unused2;
    uint8_t unused3;
    uint8_t unused4;
    uint8_t unused5;
    uint8_t unused6;
    unsigned wlp_enable:1;
    unsigned autodrain_enable:1;
} settings_t;


typedef enum run_mode_e
{
    RUN_MODE_IDLE,
    RUN_MODE_NORMAL,   
    RUN_MODE_DEGAS,
    RUN_MODE_DRAIN
} run_mode_t;



extern settings_t settings;
extern uint8_t run_mode;

extern bool ultrasonic_on;
extern bool air_enabled;
extern bool recirc_enabled;
extern bool heat_on;
extern bool heater_enabled;


extern uint8_t autodrain_timer;
extern uint8_t degas_pulse_timer;
extern uint8_t temperatures[2];

extern uint8_t out1State;
extern uint8_t out2State;
extern uint16_t inputState;
extern uint16_t elapsedTime;

void setHeater(bool enabled); // in main.c
void setRecirc(bool enabled); // in main.c
void setAir(bool enabled); // in main.c
void setMode(uint8_t new_mode); // in main.c
void resetSaveTimer(void);


void setOutputs1(uint8_t state, uint8_t mask);
void setOutputs2(uint8_t state, uint8_t mask);

#endif	/* GLOBALS_H */

