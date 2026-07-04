CC = gcc
LD = ld
AS = nasm

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib -Ikernel

OBJS = boot.o irq.o kernel.o terminal.o keyboard.o shell.o pic.o idt.o fs.o

all: os.iso

boot.o:
	$(AS) -f elf32 boot/kernel.asm -o boot.o

irq.o:
	$(AS) -f elf32 boot/irq.asm -o irq.o

kernel.o:
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o

terminal.o:
	$(CC) $(CFLAGS) -c kernel/terminal.c -o terminal.o

keyboard.o:
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o keyboard.o

shell.o:
	$(CC) $(CFLAGS) -c kernel/shell.c -o shell.o

pic.o:
	$(CC) $(CFLAGS) -c kernel/pic.c -o pic.o

idt.o:
	$(CC) $(CFLAGS) -c kernel/idt.c -o idt.o

fs.o:
	$(CC) $(CFLAGS) -c kernel/fs.c -o fs.o

kernel.bin: $(OBJS)
	$(LD) -m elf_i386 -T linker.ld -o kernel.bin $(OBJS)

os.iso: kernel.bin
	mkdir -p /tmp/myos_iso/boot/grub
	cp kernel.bin /tmp/myos_iso/boot/
	cp grub.cfg /tmp/myos_iso/boot/grub/
	grub-mkrescue -o /tmp/os.iso /tmp/myos_iso

run: os.iso
	qemu-system-i386 -cdrom /tmp/os.iso -no-reboot -no-shutdown

clean:
	rm -rf *.o *.bin /tmp/myos_iso /tmp/os.iso