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



## Known Issue
There is a bug that I haven't been able to fix yet — if you figure it out, feel free to open a PR and continue from there.
This is an open source operating system. It is still in early development and I am actively working on it.

## Contributing
Pull requests are welcome. If you find a bug or want to add a feature, feel free to fork and contribute.

## License
MIT
