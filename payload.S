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

    .global prxshot_save_argp
    .global prxshot_stack_func
    .global prxshot_return_addr

    .ent prxshot_stack_func
prxshot_stack_func:
    syscall 0xC0DE
    addiu $sp, $sp,-8
    lw    $a0, 0($sp)
    lw    $a1, 4($sp)
prxshot_return_addr:
    j     prxshot_stack_func
    addiu $sp, $sp,4
    .end prxshot_stack_func

    .ent prxshot_save_argp
prxshot_save_argp:
    addiu $a2,$a2, -8 // $a2 == user mode $sp
    sw    $a0, 0($a2) // save args
    sw    $a1, 4($a2) // save argp
    addiu $sp,$sp,-4
    sw    $ra,0($sp)  // save $ra
    jal   syscall_save_argp
    nop
    lw    $ra,0($sp)  // restore $ra
    jr    $ra
    addiu $sp,$sp,4
    .end prxshot_save_argp
