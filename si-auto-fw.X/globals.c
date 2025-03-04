
#include "globals.h"



settings_t settings = {
    .set_temp = 0,
    .degas_time = 0,
    .set_time = 0,
    .autodrain_time = 30,
    .degas_pulse_off = 10,
    .degas_pulse_on = 5,
    .wlp_enable = 1,
    .autodrain_enable = 0
};

uint8_t run_mode = RUN_MODE_IDLE;

bool ultrasonic_on = false;
bool air_enabled = false;
bool recirc_enabled = false;
bool heat_on = false;
bool heater_enabled = false;

uint8_t autodrain_timer = 0;
uint8_t degas_pulse_timer = 0;
uint8_t temperatures[2] = {0};

uint8_t out1State = 0;
uint8_t out2State = 0;
uint16_t inputState = 0;

uint16_t elapsedTime = 0;
