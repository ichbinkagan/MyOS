ENTRY(_start)

SECTIONS
{
    . = 1M;   /* load kernel at 1MB — standard for multiboot kernels */

    .text : {
        KEEP(*(.multiboot))   /* keep multiboot header even if linker thinks it's unused */
        *(.text*)             /* all code sections */
    }

    .rodata : { *(.rodata*) }   /* read-only data (string literals etc.) */
    .data   : { *(.data*)   }   /* initialized global variables */

    .bss : {
        *(COMMON)    /* uninitialized globals */
        *(.bss*)     /* zero-initialized data */
    }
}