#include "labs/shell.h"
#include "labs/vgatext.h"

#define NUM_OPTIONS 3

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
    state.computation_done = 1;
    state.coroutine=0;
    state.co_ans=1;
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
        stateinout.coroutine=0;
        stateinout.co_ans=1;
    }
    else if(stateinout.choosen_option==0) {
        if(scankey==0x48 && stateinout.selected_option>1) {
            stateinout.selected_option--;
        }
        else if(scankey==0x50 && stateinout.selected_option<NUM_OPTIONS) {
            stateinout.selected_option++;
        }
        else if(scankey==0x02) {
            stateinout.selected_option = 1;
        }
        else if(scankey==0x03) {
            stateinout.selected_option = 2;
        }
        else if(scankey==0x04) {
            stateinout.selected_option = 3;
        }
        else if(scankey==0x1c) {
            stateinout.choosen_option = stateinout.selected_option;
        }
    }
    else {
        if(scankey == 0x1c) {
            if((stateinout.choosen_option == 1 || stateinout.choosen_option==2) && stateinout.input_num>300000000)
                stateinout.illegal_input = 1;
            else {
                stateinout.computation_done = 0;
                stateinout.choosen_function = stateinout.choosen_option;
            }
        }
        else if(scankey>=0x2 && scankey<=0xb) {
            stateinout.choosen_function = 0;
            if(stateinout.input_num<=214748364) {
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

uint32_t calcPower(uint32_t a, uint32_t b);
uint32_t calcFactorial(uint32_t num);
uint32_t isPrime(uint32_t num);

//
// do computation
//
void shell_step(shellstate_t& stateinout){
    if(stateinout.choosen_function!=0 && stateinout.computation_done==0) {
        switch(stateinout.choosen_function) {
            case 1: stateinout.result = calcPower(7,stateinout.input_num);
                    stateinout.computation_done = 1;
                    break;
            case 2: stateinout.result = calcFactorial(stateinout.input_num);
                    stateinout.computation_done = 1;
                    break;
            // case 3: stateinout.result = isPrime(stateinout.input_num);
            //         stateinout.computation_done = 1;
            //         break;

            case 3: stateinout.coroutine=1;
                    stateinout.computation_done=1;
                    // hoh_debug("----");
                    
                    // hoh_debug(stateinout.co_ans);
                    // hoh_debug("----");

                    //hoh_debug(stateinout.co_ans);
                    //break;

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
    render.coroutine=shell.coroutine;
    render.co_ans=shell.co_ans;
}



//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
    if(b.num_keys_pressed!=a.num_keys_pressed)
        return false;
    if(b.selected_option!=a.selected_option)
        return false;
    if(b.choosen_option!=a.choosen_option)
        return false;
    if(b.input_num!=a.input_num)
        return false;
    if(b.overflow!=a.overflow)
        return false;
    if(b.choosen_function!=a.choosen_function)
        return false;
    if(b.result!=a.result)
        return false;
    if(b.illegal_input!=a.illegal_input)
        return false;

    if(b.coroutine!=a.coroutine)
    {   hoh_debug("diff");
        return false;
    }
    return true;
}




static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
void reset_screen(const renderstate_t& state, int w, int h, addr_t vgatext_base);
int int2str(uint32_t n, char* str);

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){

    reset_screen(state, w, h, vgatext_base);
    char str[10];
    int i = int2str(state.num_keys_pressed, str);
    drawtext(10,1," Menu",10,0x9,0x0,w,h,vgatext_base);
    drawtext(1,3,"1. Calculate power of 7",30,0x0,0x7,w,h,vgatext_base);
    drawtext(1,4,"2. Factorial",30,0x0,0x7,w,h,vgatext_base);
    drawtext(1,5,"3. Long computation new",30,0x0,0x7,w,h,vgatext_base);
    drawtext(w-25-i,h-1," Number of key presses: ",24,0x3,0x0,w,h,vgatext_base);
    drawtext(w-1-i,h-1,str,i,0x3,0x0,w,h,vgatext_base);
    drawtext(w-1,h-1," ",1,0x3,0x0,w,h,vgatext_base);

    switch(state.selected_option) {
        case 1: drawtext(1,3,"1. Calculate power of 7",30,0x7,0x0,w,h,vgatext_base); break;
        case 2: drawtext(1,4,"2. Factorial",30,0x7,0x0,w,h,vgatext_base); break;
        case 3: drawtext(1,5,"3. Long computation new",30,0x7,0x0,w,h,vgatext_base); break;
    }
    switch(state.choosen_option) {
        case 1: drawtext(1,10,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base); 
                if(state.overflow == 1)
                    drawtext(1,11,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
                if(state.illegal_input == 1)
                    drawtext(1,11,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
                else if(state.choosen_function == 1) {
                    int j = int2str(state.input_num, str);
                    drawtext(1,11,"7^",100,0x0,0x2,w,h,vgatext_base);
                    drawtext(3,11,str,j,0x0,0x2,w,h,vgatext_base);
                    drawtext(j+3,11," modulo 10^9+7: ",100,0x0,0x2,w,h,vgatext_base);
                    i = int2str(state.result, str);
                    drawtext(j+19,11,str,i,0x0,0x2,w,h,vgatext_base);
                }
                i = int2str(state.input_num, str);
                drawtext(34,10,str,i,0x0,0x7,w,h,vgatext_base);
                break;
        case 2: drawtext(1,10,"Enter a number less than 3*10^8: ",50,0x0,0x6,w,h,vgatext_base);
                if(state.overflow == 1)
                    drawtext(1,11,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
                if(state.illegal_input == 1)
                    drawtext(1,11,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
                else if(state.choosen_function == 2) {
                    i = int2str(state.result, str);
                    drawtext(1,11,"Factorial modulo 10^9+7: ",50,0x0,0x2,w,h,vgatext_base);
                    drawtext(26,11,str,i,0x0,0x2,w,h,vgatext_base);
                }
                i = int2str(state.input_num, str);
                drawtext(34,10,str,i,0x0,0x7,w,h,vgatext_base);
                break;
        case 3: drawtext(1,10,"Enter a number: ",50,0x0,0x6,w,h,vgatext_base);
                if(state.overflow == 1)
                    drawtext(1,11,"Overflow!!!",50,0x0,0x4,w,h,vgatext_base);
                if(state.illegal_input == 1)
                    drawtext(1,11,"Illegal Input!!",50,0x0,0x4,w,h,vgatext_base);
                else if(state.choosen_function == 3) {
                    i = int2str(state.input_num, str);
                    //drawtext(1,11,str,i,0x0,0x2,w,h,vgatext_base);
                   

                    if(state.coroutine==0){
                    char* x;
                    int p=int2str(state.co_ans,x);
                    hoh_debug(state.coroutine);
                    
                    drawtext(i+1,11,x,50,0x0,0x2,w,h,vgatext_base);}
                    // if(state.result==1)
                    //     drawtext(i+1,11," is prime",50,0x0,0x2,w,h,vgatext_base);
                    // else
                    //     drawtext(i+1,11," is not prime",50,0x0,0x2,w,h,vgatext_base);
                }
                i = int2str(state.input_num, str);
                drawtext(17,10,str,i,0x0,0x7,w,h,vgatext_base);

                break;
        default: drawtext(1,10,"Choose an option and press Return",50,0x0,0x3,w,h,vgatext_base); break;

    }
}




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

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
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

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
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
    int i, flag=0;
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
        ans=(ans*num)%1000000007;
        num--;
    }
   return ans;
}

uint32_t calcPower(uint32_t a, uint32_t num) {
    uint32_t ans=1;
    while(num>0) {
        ans=(a*ans)%1000000007;
        num--;
    }
    return ans;
}