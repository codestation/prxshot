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

    .set noreorder

    .global asm_hook_func
    .global asm_hook_syscall_addr
    .global asm_hook_return_addr
    .global asm_hook_end
    .ent asm_hook_func

asm_hook_func:
    addiu $sp,$sp,-12
    // save the $a0, $a1 (args/argp) and $ra registers
    sw   $ra,0($sp)
    sw   $a0,4($sp)
    sw   $a1,8($sp)
asm_hook_syscall_addr:
    // dummy syscall to be patched with our syscall number
    syscall 0xC0DE
    nop
    // restore the $a0, $a1 (args/argp) and $ra registers
    lw   $a1,8($sp)
    lw   $a0,4($sp)
    lw   $ra,0($sp)
    addiu $sp,$sp,12
asm_hook_return_addr:
    // dummy jump to be patched with the address of the module_start
    j    asm_hook_return_addr
    nop
asm_hook_end:
    .end asm_hook_func
