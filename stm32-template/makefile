
#TOOLCHAIN = C:\GNU_Tools_ARM_Embedded\7_2017-q4-major\bin

TARGET = main

CC	 = arm-none-eabi-gcc.exe
CPP	 = arm-none-eabi-g++.exe
SIZE = arm-none-eabi-size.exe
OBJCOPY = arm-none-eabi-objcopy.exe

LINKERSCRIPT = STM32F103XB_FLASH.ld
INCLUDEDIR   = -I. -I./CMSIS/Include -I./CMSIS/core 
DEFINES      = -DSTM32F103xB

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual a
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#  -adhlns...:   create assembler listing
CXXFLAGS = -ggdb -std=c++14 -marm -mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float -O0 -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections -mno-thumb-interwork
CXXFLAGS += $(INCLUDEDIR) $(DEFINES)

# Linker flags.
# -Wl,...:       tell GCC to pass this to linker.
# -Map:          create map file
# --cref:        add cross reference to  map file
# --gc-sections  eliminate unused code and data from binaries
# -nostartfiles
LDFLAGS = $(CXXFLAGS) -T$(LINKERSCRIPT) --specs=nosys.specs -Wl,--gc-sections

C_SRC = $(wildcard *.c)
CXX_SRC = $(wildcard *.cpp)
SRCS = $(C_SRC) $(CXX_SRC)
OBJS = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o) startup.o

all: elf bin

elf: $(OBJS)
	@echo ---------------------------------
	@echo Linking
	@echo ---------------------------------
	@echo Objects: $(OBJS)
	@$(CPP) $(LDFLAGS) $(OBJS) -o $(TARGET).elf
	@$(SIZE) $(TARGET).elf

bin: elf
	@echo Generate bin file	$(TARGET).bin
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

hex: elf
	@echo Generate hex file	$(TARGET).hex
	@$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex

$(OBJS): $(SRCS) .\CMSIS\startup_stm32f103xb.s
	@echo ---------------------------------
	@echo Compiling
	@echo ---------------------------------
	@echo Compile startup
	@$(CPP) -c $(CXXFLAGS) .\CMSIS\startup_stm32f103xb.s -o startup.o
	@echo Compile sources: $(SRCS) 
	@$(CPP) -c $(CXXFLAGS) $(INCLUDEDIR) $(SRCS)

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.bin
	rm -f *.hex

