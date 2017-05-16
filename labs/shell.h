#pragma once
#include "util/config.h"
#include "util/debug.h"

#define MOD 100030001
#define NUM_OPTIONS 8
#define OUT_LINE 13
#define NUM_OF_TASKS 5
#define MAX_TASKS_PER_TYPE 3

struct ltask {
	int16_t choosen_function;
	uint32_t input_num;
	uint32_t result;
	bool computation_done;
	bool fiber_running;
};

struct shellstate_t{
	uint32_t num_keys_pressed;
	uint32_t selected_option;
	int16_t choosen_option;
	uint32_t input_num;
	int16_t overflow;
	int16_t choosen_function;
	uint32_t result;
	int8_t illegal_input;
	bool computation_done;
	bool fiber_running;
	uint16_t tasks_scheduled;
	uint16_t tasks_scheduled_type1;
	uint16_t tasks_scheduled_type2;
	uint16_t tasks_scheduled_type3;
	uint8_t task_status;
	ltask tasks[NUM_OF_TASKS];
};

struct renderstate_t{
	uint32_t num_keys_pressed;
	uint32_t selected_option;
	int16_t choosen_option;
	uint32_t input_num;
	int16_t overflow;
	int16_t choosen_function; 
	uint32_t result;
	int8_t illegal_input;
	bool computation_done;
	uint16_t tasks_scheduled;
	uint8_t task_status;
	ltask tasks[NUM_OF_TASKS];
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

