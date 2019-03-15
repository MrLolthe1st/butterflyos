CC=g++
LD=LD
CFLAGS=-nostdlib -ffreestanding -std=c++11 -c
LDFLAGS=-Ttext 0x100000
BINDIR=binaries
BINDIRP=binaries/
SRCDIR=sources
SOURCES1=kernel.cpp video.cpp memory/memory.cpp string.cpp interrupts.cpp ports.cpp storage/storage.cpp storage/ata.cpp
OBJECTS=$(addprefix $(BINDIRP), $(SOURCES1:.cpp=.o))
EXECUTABLE=kernel.bin

all: $(SOURCES1) $(EXECUTABLE)
	
$(EXECUTABLE):
#
	g++ $(SRCDIR)/memory/tlsf.c -nostdlib -fpermissive -ffreestanding -c -w -o $(BINDIR)/tlsf.o
	nasm $(SRCDIR)/caller.asm -o $(BINDIR)/caller.o -f elf32
	$(LD) $(LDFLAGS) $(BINDIR)/caller.o $(BINDIR)/tlsf.o $(OBJECTS) -o $(BINDIR)/$(EXECUTABLE)
	objcopy $(BINDIR)/$(EXECUTABLE) -O binary
	nasm $(SRCDIR)/boot.asm -o $(BINDIR)/startup
	
$(SOURCES1):
	$(CC) $(CFLAGS) $(SRCDIR)/$*.cpp -o $(BINDIR)/$*.o