# MyOS

A minimal x86 operating system kernel written from scratch in C and NASM assembly.

## Features
- Boots via GRUB (Multiboot)
- VGA text mode terminal driver
- PS/2 keyboard driver (IRQ1)
- IDT + 8259 PIC interrupt handling
- Simple shell (help, clear, hello)

## Requirements
- gcc (with multilib)
- nasm
- ld
- grub-mkrescue + xorriso
- qemu-system-i386

## Build & Run
```bash
make run
```

## Project Structure


## Contributing
Pull requests are welcome. If you find a bug or want to add a feature, feel free to fork and contribute.

## License
MIT

## Commands
- help       -- lists the commands
- clear      -- clears the screen
- pwd        -- prints the directory
- mkdir      -- creates a directory
- cd         -- go to a directory
- rm         -- removes a file NOT A DIRECTORY
- touch      -- creates a file
- ls         -- lists all directories and files
