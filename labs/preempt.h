#pragma once

#include "util/config.h"


//
// preempt_t : State for your timer/preemption handler
//
// Note:
//  We've one instance of core_t per each core.
//  To access this instance, you need to use %gs:0
//  (The entire kernel doesn't have any global/static variables)
//
// %gs:core_offset_preempt will point to start of preempt_t instance
//
// for example: 
// %gs:0 will return pointer to core_t
// %gs:core_offset_mainstack will return core_t::main_stack
// %gs:core_offset_preempt+0 will return core_t::saved_stack
//
// etc.
//
// See Definition of core_t in x86/main.h
//

struct preempt_t{
  // your data structure, if any
  addr_t saved_stack; //saves stack pointer
  char fiber_yielding_flag;
};


//
// 
// This macro is being called from x86/except.cc
//
//
// _name: label name
// _f   : C function to be called 
//        ex: we may have to do send EOI to LAPIC or PIC etc.
//
#  define  _ring0_preempt(_name,_f)                    \
  __asm(                                               \
      "  .text                                    \n\t"\
      " " STR(_name) ":                           \n\t"\
      "  pushl %edx                               \n\t"\
      "  pushl %ecx                               \n\t"\
      "  pushl %eax                               \n\t"\
      "  call " STR(_f) "                         \n\t"\
      "  popl  %eax                               \n\t"\
      "  popl  %ecx                               \n\t"\
      "  popl  %edx                               \n\t"\
      "                                           \n\t"\
      "  cmpw $0x1, %gs:" STR(core_offset_preempt) "+4 \n\t"\
      "  je finish                                \n\t"\
      "                                           \n\t"\
      "  #saving all registers                    \n\t"\
      "  pushl %ebp                               \n\t"\
      "  movl %esp, %ebp                          \n\t"\
      "  subl $512, %esp                          \n\t"\
      "  andl $0xfffffff0, %esp                   \n\t"\
      "  fxsave (%esp)                            \n\t"\
      "                                           \n\t"\
      "  pushal                                   \n\t"\
      "  pushl $1f                                \n\t"\
      "                                           \n\t"\
      "  #context switching                       \n\t"\
      "  movl %esp, %gs:" STR(core_offset_preempt) " \n\t"\
      "  movl  %gs:" STR(core_offset_mainstack) ",%esp   \n\t"\
      "                                           \n\t"\
      "  ret                                      \n\t"\
      "1:                                         \n\t"\
      "  popal                                    \n\t"\
      "  fxrstor (%esp)                           \n\t"\
      "  movl %ebp, %esp                          \n\t"\
      "  popl %ebp                                \n\t"\
      "                                           \n\t"\
      "  finish:                                  \n\t"\
      "  jmp iret_toring0                         \n\t"\
      )                                                \


