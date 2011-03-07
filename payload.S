    .set noreorder

    .global asm_hook_func
    .global asm_hook_syscall_addr
    .global asm_hook_patch_addr
    .global asm_hook_return_addr
    .global asm_hook_end
    .ent asm_hook_func

asm_hook_func:
    addiu $sp,$sp,-44
    // save the $s0-$s7, $ra and $a0-$a1 registers
    sw   $ra,0($sp)
    sw   $s0,4($sp)
    sw   $s1,8($sp)
    sw   $s2,12($sp)
    sw   $s3,16($sp)
    sw   $s4,20($sp)
    sw   $s5,24($sp)
    sw   $s6,28($sp)
    sw   $s7,32($sp)
    sw   $a0,36($sp)
    sw   $a1,40($sp)
asm_hook_syscall_addr:
    // dummy syscall to be patched with our syscall number
    syscall 0xC0DE
    nop
    // restore the $s0-$s7, $ra and $a0-$a1 registers
    lw   $a1,40($sp)
    lw   $a0,36($sp)
    lw   $s7,32($sp)
    lw   $s6,28($sp)
    lw   $s5,24($sp)
    lw   $s4,20($sp)
    lw   $s3,16($sp)
    lw   $s2,12($sp)
    lw   $s1,8($sp)
    lw   $s0,4($sp)
    lw   $ra,0($sp)
    addiu $sp,$sp,44
asm_hook_patch_addr:
    // here we patch and execute the opcode that got deleted
    // by our JMP patch
    nop
asm_hook_return_addr:
    // this jump gets patched to module_start + 4
    j    asm_hook_return_addr
    nop
asm_hook_end:
    .end asm_hook_func
