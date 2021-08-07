/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
	/** Implement this function */

	instruction* Current_INST;
	Current_INST = get_inst_info(CURRENT_STATE.PC);

	unsigned char rs, rt, rd, shamt;
	short opcode, func_code, imm;
	uint32_t target, value,pc;

	value = Current_INST->value;
	opcode = Current_INST->opcode;
	func_code = Current_INST->func_code;
	rs = Current_INST->r_t.r_i.rs;
	rt = Current_INST->r_t.r_i.rt;
	rd = Current_INST->r_t.r_i.r_i.r.rd;
	imm = Current_INST->r_t.r_i.r_i.imm;
	shamt = Current_INST->r_t.r_i.r_i.r.shamt;
	target = Current_INST->r_t.target;

	int imm_sign = 0;
	if (imm < 0) {
		imm_sign = 1;	// imm�� ������ ���
	}


		if (opcode == 0) {	// RŸ��

			if (func_code == 36) {	//and: rd= rs&rt
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 33) {	//addu: rd = rs+rt
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 8) {	//jr: pc = rs
				CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
			}
			else if (func_code == 39) {	//nor: rd = ~(rs|rt)
				CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 37) {	//or: rd = rs|rt
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 43) {	//sltu: rd = (rs<rt) ? 1:0
				CURRENT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 0) {	//sll 
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 2) {	//srl
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
				CURRENT_STATE.PC += 4;
			}
			else if (func_code == 35) {	//subu
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				CURRENT_STATE.PC += 4;
			}

		}
		else if (opcode == 2) {	//JŸ��- j     
			CURRENT_STATE.PC = (target << 2) + (CURRENT_STATE.PC & 0b11110000000000000000000000000000);
			//CURRENT_STATE.PC = (target << 2 & ((CURRENT_STATE.PC >> 28) << 28));

		}
		else if (opcode == 3) {	//JŸ��- jal ������ �ּҷ� �����ϸ鼭 ���ÿ� ���� ��ɾ��� �ּҸ� $ra�� ����
			CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
			
			CURRENT_STATE.PC = (target<<2) + (CURRENT_STATE.PC & 0b11110000000000000000000000000000);           ////////////// fact test core dump
			//CURRENT_STATE.PC = (target << 2 + ((CURRENT_STATE.PC >> 28) << 28));

		}
		else if (opcode == 0b001001) {	// ���⼭���ʹ� IŸ�� addiu    ��ȣȮ��

			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + SIGN_EX(imm);
			CURRENT_STATE.PC += 4;
		}
		else if (opcode == 0b001100) {	//andi
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
			CURRENT_STATE.PC += 4;
		}
		else if (opcode == 0b000100) {	//beq    ��ȣȮ�� �ʿ�

			if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
				CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + (SIGN_EX(imm) << 2);
			}
			else {
				CURRENT_STATE.PC += 4;
			}
		}
		else if (opcode == 0b000101) {	//bne

			if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
				CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
			}
			else {
				CURRENT_STATE.PC += 4;
			}
		}
		else if (opcode == 0b001111) {	//lui
			CURRENT_STATE.REGS[rt] = (imm << 16);
			CURRENT_STATE.PC += 4;
		}
		else if (opcode == 0b100011) {	//lw  ��ȣȮ��

			CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + SIGN_EX(imm));
			CURRENT_STATE.PC += 4;
		}
		else if (opcode == 0b001101) {	//ori imm ����Ȯ��                                              
			
			//printf("ori: %x\n", CURRENT_STATE.REGS[rs] | imm);
			CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
			CURRENT_STATE.PC += 4;																	////////////// example test core dump
		
		}
		else if (opcode == 0b001011) {	//sltiuimm ��ȣȮ��

			CURRENT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < SIGN_EX(imm)) ? 1 : 0;
			CURRENT_STATE.PC += 4;
		}
		else if (opcode == 0b101011) {	//sw imm ��ȣȮ�� 

			mem_write_32(CURRENT_STATE.REGS[rs] + SIGN_EX(imm), CURRENT_STATE.REGS[rt]);
			CURRENT_STATE.PC += 4;
		}


		if (CURRENT_STATE.PC == (0x00400000 + NUM_INST * 4)) {

			RUN_BIT = FALSE;
		}
	

}
