X16 Emulator
Computer Systems 1 Final Project Fall 2025
Clementine Penrose


Implemented Files:
Bit Handler-
bits.* 
    Within bits.h and bits.c we were tasked with implementing bit handling functions within the scope of a x16 emulator. Such functions included getting
    one or multiple bits, setting a bit, clearing a bit, sign extending, and bool functions for determining whether a 16 bit number was positive or negative.
Instruction Decoding-
instruction.*
    Within instruction.h and instruction.c we had to implement functions to take whole pieces of a instruction code and retrieve its operation code and its
    immediate bit. 
Control Unit-
control.*
    Within control.h and control.c we were tasked with implementing the control logic that handles each instruction. We were given 12 instructions that had to be
    handled: add, and, br, jmp, jsr, ld, ldi, lea, ldr,, st, sti, str. 


Given files: 
2048.obj
decode.*
io.*
main.c
trap.*
x16.*
