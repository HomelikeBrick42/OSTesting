CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wl,-subsystem,10 -nostdlib \
		 -fshort-wchar -fno-stack-protector -mno-red-zone -fno-stack-check -mgeneral-regs-only \
		 -Wall -Wextra -Werror -Wno-unused-parameter \
         -e EfiMain -mno-stack-arg-probe \
		 -Isrc -Ignu-efi/inc

all: Main.efi

SRCDIR := src

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.c)
HDR = $(call rwildcard,$(SRCDIR),*.h)

Main.efi: $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $@

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
