#include <stdio.h>
#include <stdlib.h>
#include "bits.h"
#include "control.h"
#include "instruction.h"
#include "x16.h"
#include "trap.h"
#include "decode.h"


// Update condition code based on result
void update_cond(x16_t* machine, reg_t reg) {
    uint16_t result = x16_reg(machine, reg);
    if (result == 0) {
        x16_set(machine, R_COND, FL_ZRO);
    } else if (is_negative(result)) {
        x16_set(machine, R_COND, FL_NEG);
    } else {
        x16_set(machine, R_COND, FL_POS);
    }
}



// Execute a single instruction in the given X16 machine. Update
// memory and registers as required. PC is advanced as appropriate.
// Return 0 on success, or -1 if an error or HALT is encountered.
int execute_instruction(x16_t* machine) {
    // Fetch the instruction and advance the program counter
    uint16_t pc = x16_pc(machine);
    uint16_t instruction = x16_memread(machine, pc);
    x16_set(machine, R_PC, pc + 1);

    if (LOG) {
        fprintf(LOGFP, "0x%x: %s\n", pc, decode(instruction));
    }

    // Variables we might need in various instructions
    reg_t dst, src1, src2, base;
    uint16_t result, indirect, offset, imm, cond, jsrflag, op1, op2;

    // Decode the instruction
    uint16_t opcode = getopcode(instruction);
    switch (opcode) {
        case OP_ADD:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            if (getimmediate(instruction) == 0){
                src2 = getbits(instruction, 0, 3);
                result = x16_reg(machine, src1) + x16_reg(machine, src2);
            }else{
                imm = getbits(instruction, 0, 5);
                imm = sign_extend(imm, 5);
                result = x16_reg(machine, src1) + imm;
            }
            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_AND:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            if (getimmediate(instruction) == 0){
                src2 = getbits(instruction, 0, 3);
                result = x16_reg(machine, src1) & x16_reg(machine, src2);
            }else{
                imm = getbits(instruction, 0, 5);
                imm = sign_extend(imm, 5);
                result = x16_reg(machine, src1) & imm;
            }
            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_NOT:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            result = ~x16_reg(machine, src1);
            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_BR:
            cond = getbits(instruction, 9, 3);
            int16_t offset = sign_extend(getbits(instruction, 0, 9), 9);
            uint16_t cc = x16_reg(machine, R_COND);

            if (cond == 0 || cond & cc){
                uint16_t pc = x16_pc(machine);
                x16_set(machine, R_PC, pc + offset);
            }
            break;

        case OP_JMP:
            base = getbits(instruction, 6, 3);
            x16_set(machine, R_PC, x16_reg(machine, base));
            break;

        case OP_JSR:
            uint16_t pc = x16_pc(machine);
            x16_set(machine, R_R7, pc);
            jsrflag = getbit(instruction, 11);
            if (jsrflag == 1){
                offset = sign_extend(getbits(instruction, 0, 11), 11);
                x16_set(machine, R_PC, pc +offset);
            }else{
                base = getbits(instruction, 6, 3);
                x16_set(machine, R_PC, x16_reg(machine, base));
            }
            break;

        case OP_LD:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            pc = x16_pc(machine);

            uint16_t addr = pc + offset;
            result = x16_memread(machine, addr);
            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_LDI:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            pc = x16_pc(machine);
            addr = pc + offset;

            uint16_t ptr = x16_memread(machine, addr);

            result = x16_memread(machine, ptr);
            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_LDR:
            dst = getbits(instruction, 9, 3);
            base = getbits(instruction, 6, 3);
            offset = sign_extend(getbits(instruction, 0, 6), 6);

            addr = x16_reg(machine, base) + offset;
            result = x16_memread(machine, addr);

            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_LEA:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);

            pc = x16_pc(machine);
            result = pc + offset;

            x16_set(machine, dst, result);
            if (result == 0) {
                x16_set(machine, R_COND, 0b010);
            } else if (result & 0x8000) {
                x16_set(machine, R_COND, 0b100);
            } else {
                x16_set(machine, R_COND, 0b001);
            }
            break;

        case OP_ST:
            src1 = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            pc = x16_pc(machine);
            addr = pc + offset;
            x16_memwrite(machine, addr, x16_reg(machine, src1));
            break;

        case OP_STI:
            src1 = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            pc = x16_pc(machine);
            addr = pc + offset;

            indirect = x16_memread(machine, addr);

            x16_memwrite(machine, indirect, x16_reg(machine, src1));
            break;

        case OP_STR:
            src1 = getbits(instruction, 9, 3);
            base = getbits(instruction, 6, 3);
            offset = sign_extend(getbits(instruction, 0, 6), 6);
            addr = x16_reg(machine, base) + offset;

            x16_memwrite(machine, addr, x16_reg(machine, src1));
            break;

        case OP_TRAP:
            // Execute the trap -- do not rewrite
            return trap(machine, instruction);

        case OP_RES:
        case OP_RTI:
        default:
            // Bad codes, never used
            abort();
    }

    return 0;
}
