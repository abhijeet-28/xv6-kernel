#include "labs/fiber.h"
#define MOD 100030001

void f(addr_t* pmain_stack, addr_t* pf_stack, uint32_t* pret, bool* pdone, uint32_t input, bool* running);

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt,addr_t& f_stack, addr_t f_array, uint32_t f_arraysize, dev_lapic_t& lapic){
	if(shellstate.choosen_function!=0 && shellstate.computation_done==false) {
		switch(shellstate.choosen_function) {
			case 5: if(shellstate.fiber_running==0) {
				stack_init6(f_stack, f_array, f_arraysize, &f, &main_stack, &f_stack, &shellstate.result, &shellstate.computation_done,shellstate.input_num, &shellstate.fiber_running);
				shellstate.fiber_running=1;
			}
			stack_saverestore(main_stack,f_stack);
			break;
		}
	}
}

void f(addr_t* pmain_stack, addr_t* pf_stack, uint32_t* pret, bool* pdone, uint32_t input, bool* running){
	addr_t& main_stack = *pmain_stack; 
	addr_t& f_stack    = *pf_stack;
	uint32_t& ret      = *pret;
	bool& done         = *pdone;
	uint32_t i;
	ret = 1;
	for(i=1; i<=input; i++) {
		ret=(ret*7)%MOD;
		stack_saverestore(f_stack,main_stack);
	}
	done=true;
	*running=0;

	for(;;) {
		done=true; stack_saverestore(f_stack,main_stack);
	}
}
