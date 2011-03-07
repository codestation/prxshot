/*
 * module_hook.h
 *
 *  Created on: 06/03/2011
 *      Author: code
 */

#ifndef MODULE_HOOK_H_
#define MODULE_HOOK_H_

void asm_hook_func();
void asm_hook_syscall_addr();
void asm_hook_patch_addr();
void asm_hook_return_addr();
void asm_hook_end();

#endif /* MODULE_HOOK_H_ */
