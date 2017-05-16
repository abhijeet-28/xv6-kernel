#include "labs/shell.h"
#include "labs/vgatext.h"

//
// initialize shellstate
//
void shell_init(shellstate_t& state){
	state.num_keys_pressed = 0;
	state.selected_option = 1;
	state.choosen_option = 0;
	state.input_num = 0;
	state.overflow = 0;
	state.choosen_function = 0;
	state.illegal_input = 0;
	state.computation_done = true;
	state.fiber_running = 0;
	state.tasks_scheduled = 0;
	uint16_t tasks_scheduled_type1 = 0;
	uint16_t tasks_scheduled_type2 = 0;
	uint16_t tasks_scheduled_type3 = 0;
	state.task_status = 0;
	uint16_t i;
	for(i=0;i<NUM_OF_TASKS; i++) {
		state.tasks[i].choosen_function = 0;
		state.tasks[i].input_num = 0;
		state.tasks[i].result = 0;
		state.tasks[i].computation_done = true;
		state.tasks[i].fiber_running = false;
	}
}

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//
void shell_update(uint8_t scankey, shellstate_t& stateinout){

	hoh_debug("Got: "<<unsigned(scankey));
	stateinout.num_keys_pressed++;
	if(scankey==0x1) {
		stateinout.choosen_option = 0;
		stateinout.overflow = 0;
		stateinout.input_num = 0;
		stateinout.choosen_function = 0;
		stateinout.result = 0;
		stateinout.illegal_input = 0;
		stateinout.computation_done = true;
		stateinout.fiber_running = 0;
		stateinout.task_status = 0;
	}
	else if(stateinout.choosen_option==0) {
		if(scankey==0x48 && stateinout.selected_option>1) {
			stateinout.selected_option--;
		}
		else if(scankey==0x50 && stateinout.selected_option<NUM_OPTIONS) {
			stateinout.selected_option++;
		}
		else if(scankey>1 && scankey<NUM_OPTIONS+2) {
			stateinout.selected_option = scankey-1;
		}
		else if(scankey==0x1c) {
			stateinout.choosen_option = stateinout.selected_option;
		}
	}
	else {
		if(stateinout.tasks_scheduled>0 && stateinout.choosen_option>0 && stateinout.choosen_option<6) {return;}
		else {
			if(scankey == 0x1c) {
				if(stateinout.tasks_scheduled>0 || (stateinout.choosen_option>=6 && stateinout.choosen_option<=8)) {     //if some tasks are already scheduled
					if(stateinout.tasks_scheduled<NUM_OF_TASKS) {
						if((stateinout.tasks_scheduled_type1==MAX_TASKS_PER_TYPE && stateinout.choosen_option==6) || (stateinout.tasks_scheduled_type2==MAX_TASKS_PER_TYPE && stateinout.choosen_option==7) || (stateinout.tasks_scheduled_type3==MAX_TASKS_PER_TYPE && stateinout.choosen_option==8))
							stateinout.task_status = 3;       //max tasks per menu item exceeded
						else {
							uint16_t i;
							for(i=0; i<NUM_OF_TASKS; i++) {
								if(stateinout.tasks[i].computation_done == true) {
									stateinout.tasks[i].computation_done = false;
									stateinout.tasks[i].input_num = stateinout.input_num;
									stateinout.tasks[i].choosen_function = stateinout.choosen_option;
									stateinout.tasks_scheduled++;
									if(stateinout.choosen_option==6) stateinout.tasks_scheduled_type1++;
									if(stateinout.choosen_option==7) stateinout.tasks_scheduled_type2++;
									if(stateinout.choosen_option==8) stateinout.tasks_scheduled_type3++;
									stateinout.task_status = 1;        //task successfully scheduled
									hoh_debug("added");
									break;
								}
							}
						}
					}
					else {
						stateinout.task_status = 2;   //tasks buffer full
					}
				}
				else {
					if((stateinout.choosen_option == 1 || stateinout.choosen_option==2) && stateinout.input_num>300000000)  
						stateinout.illegal_input = 1;
					else {
						stateinout.computation_done = false;
						stateinout.choosen_function = stateinout.choosen_option;
					}
				}
			}
			else if(scankey>=0x2 && scankey<=0xb) {
				stateinout.choosen_function = 0;
				if(stateinout.input_num<=420000000) {
					stateinout.illegal_input = 0;
					if(scankey==0xb)
						stateinout.input_num = stateinout.input_num*10;
					else
						stateinout.input_num = stateinout.input_num*10 + scankey-1;
				}
				else
					stateinout.overflow = 1;
			}
			else if(scankey == 0xe) {
				stateinout.input_num = stateinout.input_num/10;
				stateinout.overflow = 0;
				stateinout.illegal_input = 0;
				stateinout.choosen_function = 0;
			}
		}
	}
}

uint32_t calcPower(uint32_t a, uint32_t b);
uint32_t calcFactorial(uint32_t num);
uint32_t isPrime(uint32_t num);

//
// do computation
//
void shell_step(shellstate_t& stateinout){
	if(stateinout.choosen_function!=0 && stateinout.computation_done==false) {
		switch(stateinout.choosen_function) {
			case 1: stateinout.result = calcPower(7,stateinout.input_num);
					stateinout.computation_done = true;
					break;
			case 2: stateinout.result = calcFactorial(stateinout.input_num);
					stateinout.computation_done = true;
					break;
			case 3: stateinout.result = isPrime(stateinout.input_num);
					stateinout.computation_done = true;
					break;
		}
	}
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){
	render.num_keys_pressed = shell.num_keys_pressed;
	render.selected_option = shell.selected_option;
	render.choosen_option = shell.choosen_option;
	render.input_num = shell.input_num;
	render.overflow = shell.overflow;
	render.choosen_function = shell.choosen_function;
	render.result = shell.result;
	render.illegal_input = shell.illegal_input;
	render.computation_done = shell.computation_done;
	render.tasks_scheduled = shell.tasks_scheduled;
	render.task_status = shell.task_status;
	uint16_t i;
	for(i=0;i<NUM_OF_TASKS; i++) {
		render.tasks[i].choosen_function = shell.tasks[i].choosen_function;
		render.tasks[i].input_num = shell.tasks[i].input_num;
		render.tasks[i].result = shell.tasks[i].result;
		render.tasks[i].computation_done = shell.tasks[i].computation_done;
		render.tasks[i].fiber_running = shell.tasks[i].fiber_running;
	}
}


//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
	if(b.num_keys_pressed!=a.num_keys_pressed || b.selected_option!=a.selected_option || b.choosen_option!=a.choosen_option || b.input_num!=a.input_num || b.overflow!=a.overflow || b.choosen_function!=a.choosen_function || b.illegal_input!=a.illegal_input || b.computation_done!=a.computation_done || b.tasks_scheduled!=a.tasks_scheduled || b.task_status!=a.task_status)
		return false;
	uint16_t i;
	for(i=0;i<NUM_OF_TASKS; i++) {
		if(a.tasks[i].choosen_function!=b.tasks[i].choosen_function || a.tasks[i].input_num!=b.tasks[i].input_num || a.tasks[i].computation_done!=b.tasks[i].computation_done || a.tasks[i].fiber_running!=b.tasks[i].fiber_running)
			return false;
	}
	return true;
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
void reset_screen(const renderstate_t& state, int w, int h, addr_t vgatext_base);
int int2str(uint32_t n, char* str);

const char *int2chr[] = {"0","1","2","3","4","5","6","7","8","9"};

void print_tasks_results(const renderstate_t& state, int w, int h, addr_t vgatext_base) {
	uint16_t i;
	for(i=0;i<NUM_OF_TASKS; i++) {
		drawtext(1,OUT_LINE+3+i,"TASK  : ",50,0x0,0x9,w,h,vgatext_base);
		drawtext(6,OUT_LINE+3+i,int2chr[i+1],1,0x0,0x9,w,h,vgatext_base);
		if(state.tasks[i].computation_done == false) {
			drawtext(10,OUT_LINE+3+i,"Computing...",50,0x0,0x1,w,h,vgatext_base);
		}
		else if(state.tasks[i].choosen_function == 0) {
			drawtext(10,OUT_LINE+3+i,"No task",50,0x0,0x1,w,h,vgatext_base);
		}
		else if(state.tasks[i].choosen_function == 6) {
			char str[15];
			uint32_t j = 10 + int2str(state.tasks[i].input_num, str);
			j = int2str(state.tasks[i].input_num, str);
			drawtext(10,OUT_LINE+3+i,str,j,0x0,0x2,w,h,vgatext_base);
			if(state.tasks[i].result==1)
				drawtext(j+10,OUT_LINE+3+i," is prime",50,0x0,0x2,w,h,vgatext_base);
			else
				drawtext(j+10,OUT_LINE+3+i," is not prime",50,0x0,0x2,w,h,vgatext_base);
		}
		else if(state.tasks[i].choosen_function == 7 || state.tasks[i].choosen_function == 8) {
			char str[15];
			uint32_t j = 12 + int2str(state.tasks[i].input_num, str);
			drawtext(10,OUT_LINE+3+i,"7^",100,0x0,0x2,w,h,vgatext_base);
			drawtext(12,OUT_LINE+3+i,str,j,0x0,0x2,w,h,vgatext_base);
			drawtext(j ,OUT_LINE+3+i," modulo ",100,0x0,0x2,w,h,vgatext_base);
			int k = j + 8 + int2str(MOD, str);
			drawtext(j+8,OUT_LINE+3+i,str,100,0x0,0x2,w,h,vgatext_base);
			drawtext(k,OUT_LINE+3+i," is: ",100,0x0,0x2,w,h,vgatext_base);
			j = int2str(state.tasks[i].result, str);
			drawtext(k+5,OUT_LINE+3+i,str,j,0x0,0x2,w,h,vgatext_base);
		}
	}
}

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){
	hoh_debug(state.tasks_scheduled);
	reset_screen(state, w, h, vgatext_base);
	char str[10];
	uint32_t i = int2str(state.num_keys_pressed, str), j;
	drawtext(10,1," Menu",10,0x9,0x0,w,h,vgatext_base);
	drawtext(1,3,"1. Calculate power of 7",30,0x0,0x7,w,h,vgatext_base);
	drawtext(1,4,"2. Factorial",30,0x0,0x7,w,h,vgatext_base);
	drawtext(1,5,"3. Check prime number",30,0x0,0x7,w,h,vgatext_base);
	drawtext(1,6,"4. Calculate power of 7 (coroutine)",50,0x0,0x7,w,h,vgatext_base);
	drawtext(1,7,"5. Calculate power of 7 (fiber)",50,0x0,0x7,w,h,vgatext_base);
	drawtext(1,8,"6. Check prime number (scheduler)",50,0x0,0x7,w,h,vgatext_base);
	drawtext(1,9,"7. Calculate power of 7 (scheduler)",50,0x0,0x7,w,h,vgatext_base);
	drawtext(1,10,"8. Calculate power of 7 (preemptive scheduler)",70,0x0,0x7,w,h,vgatext_base);
	drawtext(w-25-i,h-1," Number of key presses: ",24,0x3,0x0,w,h,vgatext_base);
	drawtext(w-1-i,h-1,str,i,0x3,0x0,w,h,vgatext_base);
	drawtext(w-1,h-1," ",1,0x3,0x0,w,h,vgatext_base);

	switch(state.selected_option) {
		case 1: drawtext(1,3,"1. Calculate power of 7",30,0x7,0x0,w,h,vgatext_base); break;
		case 2: drawtext(1,4,"2. Factorial",30,0x7,0x0,w,h,vgatext_base); break;
		case 3: drawtext(1,5,"3. Check prime number",30,0x7,0x0,w,h,vgatext_base); break;
		case 4: drawtext(1,6,"4. Calculate power of 7 (coroutine)",50,0x7,0x0,w,h,vgatext_base); break;
		case 5: drawtext(1,7,"5. Calculate power of 7 (fiber)",50,0x7,0x0,w,h,vgatext_base); break;
		case 6: drawtext(1,8,"6. Check prime number (scheduler)",50,0x7,0x0,w,h,vgatext_base); break;
		case 7: drawtext(1,9,"7. Calculate power of 7 (scheduler)",50,0x7,0x0,w,h,vgatext_base); break;
		case 8: drawtext(1,10,"8. Calculate power of 7 (preemptive scheduler)",70,0x7,0x0,w,h,vgatext_base); break;
	}
	switch(state.choosen_option) {
		case 1: drawtext(1,3,"1. Calculate power of 7",30,0x5,0x0,w,h,vgatext_base); break;
		case 2: drawtext(1,4,"2. Factorial",30,0x5,0x0,w,h,vgatext_base); break;
		case 3: drawtext(1,5,"3. Check prime number",30,0x5,0x0,w,h,vgatext_base); break;
		case 4: drawtext(1,6,"4. Calculate power of 7 (coroutine)",50,0x5,0x0,w,h,vgatext_base); break;
		case 5: drawtext(1,7,"5. Calculate power of 7 (fiber)",50,0x5,0x0,w,h,vgatext_base); break;
		case 6: drawtext(1,8,"6. Check prime number (scheduler)",50,0x5,0x0,w,h,vgatext_base); break;
		case 7: drawtext(1,9,"7. Calculate power of 7 (scheduler)",50,0x5,0x0,w,h,vgatext_base); break;
		case 8: drawtext(1,10,"8. Calculate power of 7 (preemptive scheduler)",70,0x5,0x0,w,h,vgatext_base); break;
	}
	if(state.tasks_scheduled>0 || (state.choosen_option>=6 && state.choosen_option<=8)) {
		if(state.choosen_option>0 && state.choosen_option<6) {
			drawtext(1,OUT_LINE-1,"Error! Other tasks running!",50,0x0,0x4,w,h,vgatext_base); 
		}
		else if(state.choosen_option>=6 && state.choosen_option<=8){
			if(state.choosen_option==6) {
				drawtext(1,OUT_LINE-1,"Enter a number: ",50,0x0,0x6,w,h,vgatext_base);
				i = int2str(state.input_num, str);
				drawtext(17,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
				drawtext(17+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
			}
			else {
				drawtext(1,OUT_LINE-1,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base);
				i = int2str(state.input_num, str);
				drawtext(34,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
				drawtext(34+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
			}
			if(state.overflow == 1)
				drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
			else if(state.task_status == 1) {
				drawtext(1,OUT_LINE,"Task added successfully",50,0x0,0x2,w,h,vgatext_base);
			}
			else if(state.task_status == 3)
				drawtext(1,OUT_LINE,"Max 3 tasks per menu item are allowed!",70,0x0,0x4,w,h,vgatext_base);
			else if(state.task_status == 2) 
				drawtext(1,OUT_LINE,"Cannot add more tasks. Please wait for some tasks to finish.",70,0x0,0x4,w,h,vgatext_base);
		}
		else {
			drawtext(1,OUT_LINE-1,"Choose an option and press Return",50,0x0,0x3,w,h,vgatext_base);
		}
		print_tasks_results(state,w,h,vgatext_base);
	}
	else {
		switch(state.choosen_option) {
			case 1: drawtext(1,OUT_LINE-1,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base); 
					if(state.overflow == 1)
						drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
					if(state.illegal_input == 1)
						drawtext(1,OUT_LINE,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
					else if(state.choosen_function == 1) {
						j = 3+int2str(state.input_num, str);
						drawtext(1,OUT_LINE,"7^",100,0x0,0x2,w,h,vgatext_base);
						drawtext(3,OUT_LINE,str,j,0x0,0x2,w,h,vgatext_base);
						drawtext(j,OUT_LINE," modulo ",100,0x0,0x2,w,h,vgatext_base);
						int k = j + 8 + int2str(MOD, str);
						drawtext(j+8,OUT_LINE,str,100,0x0,0x2,w,h,vgatext_base);
						drawtext(k,OUT_LINE," is: ",100,0x0,0x2,w,h,vgatext_base);
						i = int2str(state.result, str);
						drawtext(k+5,OUT_LINE,str,i,0x0,0x2,w,h,vgatext_base);
					}
					i = int2str(state.input_num, str);
					drawtext(34,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
					drawtext(34+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
					break;
			case 2: drawtext(1,OUT_LINE-1,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base);
					if(state.overflow == 1)
						drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
					if(state.illegal_input == 1)
						drawtext(1,OUT_LINE,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
					else if(state.choosen_function == 2) {
						drawtext(1,OUT_LINE,"Factorial modulo ",50,0x0,0x2,w,h,vgatext_base);
						j = 18 + int2str(MOD, str);
						drawtext(18,OUT_LINE,str,100,0x0,0x2,w,h,vgatext_base);
						drawtext(j,OUT_LINE," is: ",100,0x0,0x2,w,h,vgatext_base);
						i = int2str(state.result, str);
						drawtext(j+5,OUT_LINE,str,i,0x0,0x2,w,h,vgatext_base);
					}
					i = int2str(state.input_num, str);
					drawtext(34,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
					drawtext(34+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
					break;
			case 3: drawtext(1,OUT_LINE-1,"Enter a number: ",50,0x0,0x6,w,h,vgatext_base);
					if(state.overflow == 1)
						drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
					if(state.illegal_input == 1)
						drawtext(1,OUT_LINE,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
					else if(state.choosen_function == 3) {
						i = int2str(state.input_num, str);
						drawtext(1,OUT_LINE,str,i,0x0,0x2,w,h,vgatext_base);
						if(state.result==1)
							drawtext(i+1,OUT_LINE," is prime",50,0x0,0x2,w,h,vgatext_base);
						else
							drawtext(i+1,OUT_LINE," is not prime",50,0x0,0x2,w,h,vgatext_base);
					}
					i = int2str(state.input_num, str);
					drawtext(17,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
					drawtext(17+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
					break;
			case 4: drawtext(1,OUT_LINE-1,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base);
					if(state.overflow == 1)
						drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
					if(state.illegal_input == 1)
						drawtext(1,OUT_LINE,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
					else if(state.choosen_function == 4 && state.computation_done==true) {
						j = 3+int2str(state.input_num, str);
						drawtext(1,OUT_LINE,"7^",100,0x0,0x2,w,h,vgatext_base);
						drawtext(3,OUT_LINE,str,j,0x0,0x2,w,h,vgatext_base);
						drawtext(j,OUT_LINE," modulo ",100,0x0,0x2,w,h,vgatext_base);
						int k = j + 8 + int2str(MOD, str);
						drawtext(j+8,OUT_LINE,str,100,0x0,0x2,w,h,vgatext_base);
						drawtext(k,OUT_LINE," is: ",100,0x0,0x2,w,h,vgatext_base);
						i = int2str(state.result, str);
						drawtext(k+5,OUT_LINE,str,i,0x0,0x2,w,h,vgatext_base);
					}
					else if(state.computation_done==false)
						drawtext(1,OUT_LINE,"Computing...",50,0x0,0x1,w,h,vgatext_base);
					i = int2str(state.input_num, str);
					drawtext(34,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
					drawtext(34+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
					break;
			case 5: drawtext(1,OUT_LINE-1,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base);
					if(state.overflow == 1)
						drawtext(1,OUT_LINE,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
					if(state.illegal_input == 1)
						drawtext(1,OUT_LINE,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
					else if(state.choosen_function == 5 && state.computation_done==true) {
						j = 3+int2str(state.input_num, str);
						drawtext(1,OUT_LINE,"7^",100,0x0,0x2,w,h,vgatext_base);
						drawtext(3,OUT_LINE,str,j,0x0,0x2,w,h,vgatext_base);
						drawtext(j,OUT_LINE," modulo ",100,0x0,0x2,w,h,vgatext_base);
						int k = j + 8 + int2str(MOD, str);
						drawtext(j+8,OUT_LINE,str,100,0x0,0x2,w,h,vgatext_base);
						drawtext(k,OUT_LINE," is: ",100,0x0,0x2,w,h,vgatext_base);
						i = int2str(state.result, str);
						drawtext(k+5,OUT_LINE,str,i,0x0,0x2,w,h,vgatext_base);
					}
					else if(state.computation_done==false)
						drawtext(1,OUT_LINE,"Computing...",50,0x0,0x1,w,h,vgatext_base);
					i = int2str(state.input_num, str);
					drawtext(34,OUT_LINE-1,str,i,0x0,0x7,w,h,vgatext_base);
					drawtext(34+i,OUT_LINE-1," ",1,0x7,0x0,w,h,vgatext_base);
					break;
			default: drawtext(1,OUT_LINE-1,"Choose an option and press Return",50,0x0,0x3,w,h,vgatext_base); break;
		}
	}
}


//
//
// helper functions
//
//
static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
	vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}


static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

int int2str(uint32_t n, char* str) {
	int i=0,j=0;
	char temp;
	if(n==0) {
		str[0]='0';
		str[1]='\0';
		return 1;
	}
	while(n>0) {
		str[i] = n % 10+'0';
		n/=10;
		i++;
	}
	while(j<i/2) {
		temp = str[j];
		str[j] = str[i-j-1];
		str[i-j-1] = temp;
		j++;
	}
	str[i]='\0';
	return i;
}

void reset_screen(const renderstate_t& state, int w, int h, addr_t vgatext_base) {
	int i;
	for(i=0; i<h; i++)
		fillrect(0,i,w,i+1,0x0,0x0,w,h,vgatext_base);
}

uint32_t isPrime(uint32_t num) {
	uint32_t i, flag=0;
	if(num==1 || num==0)
		return 0;
	for(i=2; i<num; i++)
		if(num%i==0)
			flag=1;
	if(flag==1)
		return 0;
	return 1;
}

uint32_t calcFactorial(uint32_t num) {
	uint32_t ans=1;
	while(num>0) {
		ans=(ans*num)%MOD;
		num--;
	}
	return ans;
}

uint32_t calcPower(uint32_t a, uint32_t num) {
	uint32_t ans=1;
	while(num>0)
	{
		ans=(a*ans)%MOD;
		num--;
	}
	return ans;
}
