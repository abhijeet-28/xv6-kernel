#pragma once

#include "labs/shell.h"
#include "util/coroutine.h"

// state for your coroutine implementation:
struct f_t{
	uint32_t loopindex;
	bool flag;
	uint32_t ans;
};


void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals);


