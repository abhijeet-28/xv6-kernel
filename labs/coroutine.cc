#include "labs/coroutine.h"
#include "labs/shell.h"

void isPrime_CR(coroutine_t* pf_coro, f_t* pf_locals, uint32_t* pres, bool* pdone, uint32_t num);
void calcPower7_CR(coroutine_t* pf_coro, f_t* pf_locals, uint32_t* pres, bool* pdone, uint32_t num);

void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){
	if(shellstate.choosen_function!=0 && shellstate.computation_done==false) {
		switch(shellstate.choosen_function) {
			// case 4: isPrime_CR(&f_coro,&f_locals,&shellstate.result,&shellstate.computation_done,shellstate.input_num);
					// break;
			case 4: calcPower7_CR(&f_coro,&f_locals,&shellstate.result,&shellstate.computation_done,shellstate.input_num);
					break;
		}
	}
}

void isPrime_CR(coroutine_t* pf_coro, f_t* pf_locals, uint32_t* pres, bool* pdone, uint32_t num) {
	coroutine_t& f_coro = *pf_coro;
	uint32_t& res = *pres;
	bool& done = *pdone;
	uint32_t& i = pf_locals->loopindex;
	bool& flag = pf_locals->flag;
	h_begin(f_coro);
	flag=0;
	if(num==1 || num==0 ) {
		res=0;
		done=true;
		coroutine_reset(f_coro);
	}
	for(i=2; i<num; i++) {
		if(num%i==0) {
			flag=1;
		} 
		h_yield(f_coro);
	}
	if(flag==1)
		res = 0;
	else
		res = 1;
	done=true;
	coroutine_reset(f_coro);
}

void calcPower7_CR(coroutine_t* pf_coro, f_t* pf_locals, uint32_t* pres, bool* pdone, uint32_t num) {
	coroutine_t& f_coro = *pf_coro;
	uint32_t& res = *pres;
	bool& done = *pdone;
	uint32_t& i = pf_locals->loopindex;
	uint32_t& ans = pf_locals->ans;
	h_begin(f_coro);
	ans=1;
	for(i=1; i<=num; i++) {
		ans=(ans*7)%MOD;
		h_yield(f_coro);
	}
	res = ans;
	done=true;
	coroutine_reset(f_coro);
}

