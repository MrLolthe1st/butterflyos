#    	ButterflyOS makefile


#                    `         '
#    ;,,,             `       '             ,,,;
#    `YES8888bo.       :     :       .od8888YES'
#      888IO8DO88b.     :   :     .d8888I8DO88
#      8LOVEY'  `Y8b.   `   '   .d8Y'  `YLOVE8
#     jTHEE!  .db.  Yb. '   ' .dY  .db.  8THEE!
#       `888  Y88Y    `b ( ) d'    Y88Y  888'
#        8MYb  '"        ,',        "'  dMY8
#       j8prECIOUSgf"'   ':'   `"?g8prECIOUSk
#         'Y'   .8'     d' 'b     '8.   'Y'
#          !   .8' db  d'; ;`b  db '8.   !
#             d88  `'  8 ; ; 8  `'  88b
#            d88Ib   .g8 ',' 8g.   dI88b
#           :888LOVE88Y'     'Y88LOVE888:
#           '! THEE888'       `888THEE !'
#              '8Y  `Y         Y'  Y8'
#               Y                   Y
#              !                   !


#    	Compiler

DRIVE_LETTER	=	A:
CC				=	g++
LD				=	LD

#    	Options
CFLAGS			=	-nostdlib -ffreestanding -std=c++11 -c -O0
LDFLAGS			=	-Ttext 0x100000
#    	Directories
BINDIR			=	binaries
SRCDIR			=	sources

#    	TODO: auto .cpp collection
SOURCES1		=	kernel.cpp video.cpp memory/memory.cpp string.cpp interrupts.cpp ports.cpp storage/storage.cpp storage/ata.cpp input/input.cpp input/ps2.cpp pci.cpp storage/ahci.cpp std/trees.cpp std/map.cpp storage/drives.cpp usb/usbd.cpp usb/ehci.cpp link.cpp storage/msd.cpp usb/hub.cpp

#    	To all files: add $(BINDIR)/ to filename and replace extension
OBJ				=	$(addprefix $(BINDIR)/, $(SOURCES1:.cpp=.o))
EXECUTABLE		=	kernel.bin

SYNC			:=
ifeq ($(OS), Windows_NT)
	SYNC := sync64.exe $(DRIVE_LETTER)
endif
all: $(EXECUTABLE)
	@echo Up-to-date

#     cpp files
$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(eval temp := $*)
	@$(CC) $(CFLAGS) $(SRCDIR)/$*.cpp -o $(BINDIR)/$*.o

#    Additional dependeces.
$(BINDIR)/caller.o: $(SRCDIR)/caller.asm
	nasm $(SRCDIR)/caller.asm -o $(BINDIR)/caller.o -f elf32

$(BINDIR)/tlsf.o: $(SRCDIR)/memory/tlsf.c
	g++ $(SRCDIR)/memory/tlsf.c -nostdlib -fpermissive -ffreestanding -c -w -o $(BINDIR)/tlsf.o

$(EXECUTABLE): $(BINDIR)/tlsf.o $(BINDIR)/caller.o $(OBJ)
	@$(LD) $(LDFLAGS) $(BINDIR)/caller.o $(BINDIR)/tlsf.o $(OBJ) -o $(BINDIR)/$(EXECUTABLE)
	@objcopy $(BINDIR)/$(EXECUTABLE) -O binary
	@nasm $(SRCDIR)/boot.asm -o $(BINDIR)/startup -w-lock
	@echo Builded

run: all
	@copy $(BINDIR)\startup $(DRIVE_LETTER)\STARTUP
	@$(SYNC)
	@copy images\disk.img images\boot.img
	qemu-system-x86_64.exe -m 92 -net nic,model=rtl8139   -drive if=none,id=usbstick,file=images\boot.img -usb -device usb-ehci,id=ehci -device usb-storage,bus=ehci.0,drive=usbstick -no-reboot -vga std -D aa.txt -monitor stdio -device usb-mouse 