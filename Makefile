tinyos.iso: tinyos.bin
	mkdir -p isodir
	cp tinyos.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o tinyos.iso isodir/

tinyos.bin: obj/boot.o obj/kernel.o
	i686-elf-gcc -T linker.ld -o tinyos.bin -O0 -nostdlib obj/boot.o obj/kernel.o -lgcc

obj/boot.o: objdir src/boot.asm
	nasm -f elf32 src/boot.asm -o obj/boot.o

obj/kernel.o: objdir src/kernel.c
	i686-elf-gcc -c src/kernel.c -o obj/kernel.o -O0 -Wall -Wextra -g

objdir:
	mkdir -p obj/