    .set noreorder

    .global asm_hook_func
    .global asm_hook_syscall_addr
    .global asm_hook_return_addr
    .global asm_hook_end
    .ent asm_hook_func

asm_hook_func:
    addiu $sp,$sp,-12
    // save the $a0, $a1 and $ra registers
    sw   $ra,0($sp)
    sw   $a0,4($sp)
    sw   $a1,8($sp)
asm_hook_syscall_addr:
    // dummy syscall to be patched with our syscall number
    syscall 0xC0DE
    nop
    // restore the $a0, $a1 and $ra registers
    lw   $a1,8($sp)
    lw   $a0,4($sp)
    lw   $ra,0($sp)
    addiu $sp,$sp,12
asm_hook_return_addr:
    j    asm_hook_return_addr
    nop
asm_hook_end:
    .end asm_hook_func
