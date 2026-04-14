MEMORY {
   
    flash (rx) : ORIGIN = 0x8000000, LENGTH = 64K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_eram = ORIGIN(ram) + LENGTH(ram);
_stack = _eram; /* Placed at the end of ram and grows downwards */

SECTIONS {
    
    .text : ALIGN(4) {

        KEEP(*(.ivt))
        *(.text*)
        . = ALIGN(4);
        _etext = .;
    
    } > flash


    _idata = LOADADDR(.data);


    .data : ALIGN(4) {
        
        *(.data*)
        . = ALIGN(4);
        _edata = .;

    } > ram AT > flash

    .bss : ALIGN(4) {
        
        _bss = .;
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
        PROVIDE(_end = .);

    } > ram
}

