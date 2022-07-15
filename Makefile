CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wl,-subsystem,10 -nostdlib \
		 -fshort-wchar -fno-stack-protector \
		 -Wall -Wextra -Werror -Wno-unused-parameter \
         -e EfiMain

all: Main.efi

%.efi: %.c
	$(CC) $(CFLAGS) $< -o $@

qemu: Main.efi OVMF.fd image/EFI/BOOT/BOOTX64.EFI
	qemu-system-x86_64 -bios OVMF.fd -drive file=fat:rw:image,media=disk,format=raw

image/EFI/BOOT/BOOTX64.EFI:
	mkdir -p image/EFI/BOOT
	ln -sf ../../../Main.efi image/EFI/BOOT/BOOTX64.EFI

OVMF.fd:
	wget http://downloads.sourceforge.net/project/edk2/OVMF/OVMF-X64-r15214.zip
	unzip OVMF-X64-r15214.zip OVMF.fd
	rm OVMF-X64-r15214.zip

clean:
	rm -f Main.efi OVMF.fd
	rm -rf image
