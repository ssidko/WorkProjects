
TARGET = main

CC	 = arm-none-eabi-gcc.exe
CXX	 = arm-none-eabi-g++.exe
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
CXXFLAGS = -ggdb -O0 -std=c++14 -marm -mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections -mno-thumb-interwork
CXXFLAGS += $(INCLUDEDIR) $(DEFINES)

# Linker flags.
# -Wl,...:       tell GCC to pass this to linker.
# -Map:          create map file
# --cref:        add cross reference to  map file
# --gc-sections  eliminate unused code and data from binaries
# -nostartfiles
LDFLAGS = $(CXXFLAGS) -T$(LINKERSCRIPT) --specs=nosys.specs -Wl,--gc-sections

#VPATH = ./CMSIS ./app
#C_SRC = $(wildcard *.c)
#CXX_SRC = $(wildcard *.cpp)
#ASM_SRC = $(wildcard ./*.s) $(wildcard ./CMSIS/*.s)
#SRCS = $(C_SRC) $(CXX_SRC) $(ASM_SRC)
#OBJS = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o) $(ASM_SRC:.s=.o)

OBJ_DIR     = ./.obj
src_dirs    = . ./CMSIS ./app ./drivers
INCLUDEDIR  += $(patsubst %,-I%, $(src_dirs))

cpp_files   = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.cpp))
c_files     = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.c))
asm_files   = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.s))

all_sources = $(cpp_files) $(c_files) $(asm_files)
all_objects = $(foreach file, $(all_sources), $(OBJ_DIR)/$(basename $(file)).o)


all: info elf


elf: $(all_objects)
	@echo ---------------------------------
	@echo "*** Linking ***"
	@echo ---------------------------------
	@$(CXX) $(LDFLAGS) $(all_objects) -o $(TARGET).elf
	@$(SIZE) $(TARGET).elf

info:
	@echo
	@echo include: $(INCLUDEDIR)
	@echo
	@echo cpp_files: $(cpp_files)
	@echo c_files: $(c_files)
	@echo asm_files: $(asm_files)
	@echo
	@echo all_sources: $(all_sources)
	@echo all_objects: $(all_objects)
	@echo

bin: elf
	@echo Generate binary file:	$(TARGET).bin
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

hex: elf
	@echo Generate hex file: $(TARGET).hex
	@$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo Compile: $< 
	@$(CXX) -c $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo Compile: $<
	@$(CXX) -c $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(@D)
	@echo Compile: $<
	@$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@echo 
	@echo Cleaninig 
	@rm -fr $(OBJ_DIR)
	@rm -f *.o
	@rm -f *.elf
	@rm -f *.bin
	@rm -f *.hex
