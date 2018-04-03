.section ".text.boot"             

.global _start                    

_start:
	
// ONLY FOR MODEL 2 !!!!!!! sending 3 of 4 cores to halt
 //   mrc p15, #0, r1, c0, c0, #5
 //   and r1, r1, #3
 //   cmp r1, #0
 //   bne halt

// put the beginning of the C-stack (core starts at 0x8000) 
    mov sp, #0x8000

// C global variables that are not initialized at compile time are stored in BSS
// We should set all of them to 0
    //ldr r4, =__bss_start

	//    ldr r9, =__bss_end

    //mov r5, #0
    //mov r6, #0
    //mov r7, #0
    //mov r8, #0
    //b       2f

//1:
  //  stmia r4!, {r5-r8}

//2:
  //  cmp r4, r9
    //blo 1b

// jumps to the C-function kernel_main and halts after it returned
    mov r2, #0x100
    bl kernel_main
    b halt

halt:
    wfe
    b halt
