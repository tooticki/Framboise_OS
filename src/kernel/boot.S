.section ".text.boot"             

.global _start                    

_start:	// put the beginning of the C-stack (core starts at 0x8000) 
	mov sp, #0x8000

	// atags are loaded at address 0x100 by default; pass them at 3rd
	// argument to C-function kernel_main
	mov r2, #0x100
	bl kernel_main

halt:	wfe
	b halt
