#pragma once
#include "util/config.h"
#include "util/debug.h"

struct shellstate_t{
    int32_t num_keys_pressed;
    int32_t selected_option;
    int16_t choosen_option;
    int32_t input_num;
    int16_t overflow;
    int16_t choosen_function;
    int32_t result;
    int8_t illegal_input;
    int8_t computation_done;
    int8_t coroutine;
    int32_t co_ans;
};

struct renderstate_t{
    int32_t num_keys_pressed;
    int32_t selected_option;
    int16_t choosen_option;
    int32_t input_num;
    int16_t overflow;
    int16_t choosen_function; 
    int32_t result;
    int8_t illegal_input;
    int8_t computation_done;
    int8_t coroutine;
    int32_t co_ans;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

