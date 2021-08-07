/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

char* my_substr(int start, int dest, char* str) {
    char* new = (char*)malloc(sizeof(char) * (dest - start + 2));
    strncpy(new, str + start, dest - start + 1);
    new[dest - start + 1] = 0;
    return new;
}

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
	/** Implement this function */

    short op, funct, immed;
    int rs, rt, rd, shamt;
    unsigned short imm;

    uint32_t value = fromBinary(buffer);    //value는 instruciton bit 값 
    instr.value = value;
    //printf("value %x\ninstr.value: %x\n", value, instr.value);
    mem_write_32(MEM_TEXT_START + index, value);

    op = (value & 0b11111100000000000000000000000000) >> 26;
    instr.opcode = op;

    if (op == 0) {    // R 타입

        rs = (value & 0b00000011111000000000000000000000) >> 21;
        rt = (value & 0b00000000000111110000000000000000) >> 16;
        rd = (value & 0b00000000000000001111100000000000) >> 11;
        shamt = (value & 0b00000000000000000000011111000000) >> 6;
        funct = value & 0b00000000000000000000000000111111;

        instr.r_t.r_i.rs = rs; instr.r_t.r_i.rt = rt; instr.r_t.r_i.r_i.r.rd = rd; instr.r_t.r_i.r_i.r.shamt = shamt; instr.func_code = funct;

    }
    else if (op == 2 || op == 3) {  // J 타입

        uint32_t target = value & 0b00000011111111111111111111111111;
        
        instr.r_t.target = target;

    }
    else {  // I 타입

        rs = (value & 0b00000011111000000000000000000000) >> 21;
        rt = (value & 0b00000000000111110000000000000000) >> 16;
        immed = value & 0b00000000000000001111111111111111;

        //printf("immed: %d\n", immed);
        instr.r_t.r_i.rs = rs; instr.r_t.r_i.rt = rt; instr.r_t.r_i.r_i.imm = immed;
        //printf("instr.r_t.r_i.r_i.imm : %d\n", instr.r_t.r_i.r_i.imm);

    }
    
    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	/** Implement this function */

    uint32_t value = fromBinary(buffer);
    mem_write_32(MEM_DATA_START + index, value);
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
