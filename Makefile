#ButterfluOS makefile
#Compiler
CC=g++
LD=LD
#Options
CFLAGS=-nostdlib -ffreestanding -std=c++11 -c -O0
LDFLAGS=-Ttext 0x100000
#Directorries
BINDIR=binaries
SRCDIR=sources
SOURCES1=kernel.cpp video.cpp memory/memory.cpp string.cpp interrupts.cpp ports.cpp storage/storage.cpp storage/ata.cpp input/input.cpp input/ps2.cpp pci.cpp storage/ahci.cpp std/trees.cpp std/map.cpp storage/drives.cpp usb/usbd.cpp usb/ehci.cpp link.cpp storage/msd.cpp usb/hub.cpp
OBJ=$(addprefix $(BINDIR)/, $(SOURCES1:.cpp=.o))
EXECUTABLE=kernel.bin

all: $(EXECUTABLE)
	@echo Up-to-date

# cpp files
$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(eval temp := $*)
	@$(CC) $(CFLAGS) $(SRCDIR)/$*.cpp -o $(BINDIR)/$*.o

#Additional dependeces.
$(BINDIR)/caller.o: $(SRCDIR)/caller.asm
	nasm $(SRCDIR)/caller.asm -o $(BINDIR)/caller.o -f elf32
$(BINDIR)/tlsf.o: $(SRCDIR)/memory/tlsf.c
	g++ $(SRCDIR)/memory/tlsf.c -nostdlib -fpermissive -ffreestanding -c -w -o $(BINDIR)/tlsf.o

$(EXECUTABLE): $(BINDIR)/tlsf.o $(BINDIR)/caller.o $(OBJ)
	@$(LD) $(LDFLAGS) $(BINDIR)/caller.o $(BINDIR)/tlsf.o $(OBJ) -o $(BINDIR)/$(EXECUTABLE)
	@objcopy $(BINDIR)/$(EXECUTABLE) -O binary
	@nasm $(SRCDIR)/boot.asm -o $(BINDIR)/startup -w-lock
	@echo Builded
