#include "labs/fiber_scheduler.h"

void power7_preempt(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate, preempt_t* preempt, int j);
void power7(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate, preempt_t* preempt, int j);
void prime(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate,preempt_t* preempt,int j);

void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t& mainstack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t& lapic){

	if(shellstate.tasks_scheduled>0) {
		uint32_t i;
		for(i=0;i<NUM_OF_TASKS; i++) {
			if(shellstate.tasks[i].computation_done==false) {
				if(shellstate.tasks[i].fiber_running==0) {
					switch(shellstate.tasks[i].choosen_function) {
						case 6: stack_init5(stackptrs[i], arrays+i*(arrays_size/stackptrs_size), arrays_size/stackptrs_size, &prime, &mainstack, &stackptrs[i], &shellstate, &preempt, i);
								break;
						case 7: stack_init5(stackptrs[i], arrays+i*(arrays_size/stackptrs_size), arrays_size/stackptrs_size, &power7, &mainstack, &stackptrs[i], &shellstate, &preempt, i);
								break;
						case 8: stack_init5(stackptrs[i], arrays+i*(arrays_size/stackptrs_size), arrays_size/stackptrs_size, &power7_preempt, &mainstack, &stackptrs[i], &shellstate, &preempt, i);
								break;
					}
					shellstate.tasks[i].fiber_running=1;
				}
				
				lapic.reset_timer_count(10000000);
				// hoh_debug("timer started");
				stack_saverestore(mainstack,stackptrs[i]);
				// hoh_debug("returned");
				__asm("sti");
				if(preempt.fiber_yielding_flag == 1) {
					preempt.fiber_yielding_flag = 0;
				}
				else {
					stackptrs[i] = preempt.saved_stack;
				}
			}
		}
		
	}

}


void power7_preempt(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate, preempt_t* preempt, int j) {
	addr_t& main_stack = *pmain_stack; 
	addr_t& f_stack    = *pf_stack;
	uint32_t& ret      = shellstate->tasks[j].result;
	bool& done         = shellstate->tasks[j].computation_done;
	uint32_t i, input = shellstate->tasks[j].input_num;
	ret = 1;
	
	for(i=1; i<=input; i++) {
		ret=(ret*7)%MOD;
	}
	
	done=true;
	shellstate->tasks[j].fiber_running=false;
	(shellstate->tasks_scheduled)--;
	(shellstate->tasks_scheduled_type3)--;
	
	for(;;) { }
}

void power7(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate, preempt_t* preempt, int j) {
	
	addr_t& main_stack = *pmain_stack; 
	addr_t& f_stack    = *pf_stack;
	uint32_t& ret      = shellstate->tasks[j].result;
	bool& done         = shellstate->tasks[j].computation_done;
	uint32_t i, input = shellstate->tasks[j].input_num;
	ret = 1;
	int k=0;
	for(i=1; i<=input; i++) {
		ret=(ret*7)%MOD;
		k++;
		if(k==500) {
			k=0;
			preempt->fiber_yielding_flag=1;
			stack_saverestore(f_stack,main_stack);
		}
	}
	
	done=true;
	shellstate->tasks[j].fiber_running=false;
	(shellstate->tasks_scheduled)--;
	(shellstate->tasks_scheduled_type2)--;
	
	for(;;) {
		done=true; stack_saverestore(f_stack,main_stack);
	}
}

void prime(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t* shellstate,preempt_t* preempt,int j) {

	addr_t& main_stack = *pmain_stack; 
	addr_t& f_stack    = *pf_stack;
	uint32_t& ret      = shellstate->tasks[j].result;
	bool& done         = shellstate->tasks[j].computation_done;
	uint32_t i, flag=0, input   = shellstate->tasks[j].input_num;
	
	int k=0;
	if(input < 2) {
		ret = 0;
	}
	else
		for(i=2; i<input; i++) {
			if(input%i==0) {
				flag=1;
			}
			k++;
			if(k==500) {
				k=0;
				preempt->fiber_yielding_flag=1;
				stack_saverestore(f_stack,main_stack);
			}
		}
	if(flag==1)
		ret = 0;
	else
		ret = 1;
	done=true;
	shellstate->tasks[j].fiber_running=false;
	(shellstate->tasks_scheduled)--;
	(shellstate->tasks_scheduled_type1)--;
	
	for(;;) {
		done=true; stack_saverestore(f_stack,main_stack);
	}
}
