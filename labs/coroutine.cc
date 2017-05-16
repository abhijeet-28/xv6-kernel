#include "labs/coroutine.h"


void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){

    //insert your code here
	
	if(shellstate.coroutine==1){

    uint32_t& loopindex=f_locals.loopindex;
    uint32_t& answer=f_locals.answer;
    if(loopindex==0)
    answer=1;
    h_begin(f_coro);

    //hoh_debug(loopindex);
    int32_t input_num=shellstate.input_num;
    int32_t i;
    for(loopindex=0;loopindex<=shellstate.input_num;loopindex++)
    {
    	answer=(answer*7)%1000007;
    	//hoh_debug("a");

    	hoh_debug(loopindex);
        //hoh_debug(answer);
    	shellstate.co_ans=answer;
    	//hoh_debug(shellstate.co_ans);
    	h_yield(f_coro);

    }
    
    //hoh_debug("bingo");
    //hoh_debug(loopindex);
    loopindex=0;
    answer=1;
    shellstate.coroutine=0;

}
     
    

}


