/*
 *  prxshot module
 *
 *  Copyright (C) 2011  Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HOOK_INJECT_H_
#define HOOK_INJECT_H_

#include <psploadcore.h>

#define J_OPCODE        0x08000000
#define JAL_OPCODE      0x0C000000
#define SYSCALL_OPCODE  0x0000000C
#define MV_A2SP_OPCODE  0x03A03021

#define NOP     0x00000000
#define MAKE_JUMP( a, f ) _sw( J_OPCODE | ( ( ( unsigned int )( f ) & 0x0ffffffc ) >> 2 ), a )
#define MAKE_CALL( a, f ) _sw( JAL_OPCODE | ( ( ( unsigned int )( f ) >> 2 )  & 0x03ffffff ), a )
#define MAKE_SYSCALL( a, f ) _sw(SYSCALL_OPCODE | ( ( ( unsigned int )( f ) & 0x03ffffff ) << 6), a )

void hook_module_start();
void create_stack_payload(void *user_stack);
void restore_module_start();

#endif /* HOOK_INJECT_H_ */
