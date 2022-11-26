CPP_SRCS=$(wildcard src/*.cpp)
C_SRCS=$(wildcard src/*.c)
ASM_SRCS=$(wildcard src/*.asm)
CPP_OBJS=${CPP_SRCS:.cpp=.o}
C_OBJS=${C_SRCS:.c=.o}
ASM_OBJS=${ASM_SRCS:.asm=.o}
PSF_SRCS = $(wildcard src/*.psf)
PSF_OBJS = ${PSF_SRCS:.psf=.o}
OBJS=${ASM_OBJS} ${C_OBJS} ${CPP_OBJS} ${PSF_OBJS}
kernel.elf: ${OBJS}
	ld ${OBJS} -Tkernel.ld -o kernel.elf -znocombreloc -Bsymbolic -nostdlib
	objcopy --only-keep-debug kernel.elf kernel.sym
%.o: %.cpp
	g++ -c $< -o $@ -Iinclude -fPIC -I/usr/include/efi -I/usr/include/efi/x86_64 \
		-fno-stack-protector -D_GLIBCXX_USE_CXX11_ABI=0 -fshort-wchar -fno-exceptions \
		-Wall -Wextra -g -fno-rtti -D__DEBUG__
%.o: %.c
	gcc -c $< -o $@ -Iinclude -fPIC -I/usr/include/efi -I/usr/include/efi/x86_64 \
		-fno-stack-protector -fshort-wchar -fno-exceptions -Wall -Wextra -g -D__DEBUG__
%.o: %.asm
	nasm $< -o $@ -f elf64
%.o: %.psf
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@
clean:
	rm -rf ${OBJS} kernel.elf kernel.sym