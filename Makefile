##==========================================================================
##                  vmc
##--------------------------------------------------------------------------
##==========================================================================

ARMCORE = NXP1768
APP = VMC_mini
VERSION  = 1.0
TARGET   = $(strip $(ARMCORE))$(strip $(MEMORY))-$(strip $(VERSION))

CUR_DIR = .


BUILD_DIR=$(CUR_DIR)/build
BIN_DIR=$(BUILD_DIR)/gcc
OBJ_DIR=$(BUILD_DIR)/obj
LST_DIR=$(BUILD_DIR)/lst

GCC_PATH=C:/GNUToolsArm
CC=$(GCC_PATH)/bin/arm-none-eabi-gcc
AS=$(GCC_PATH)/bin/arm-none-eabi-as
AR=$(GCC_PATH)/bin/arm-none-eabi-ar
LD=$(GCC_PATH)/bin/arm-none-eabi-ld
OBJCOPY=$(GCC_PATH)/bin/arm-none-eabi-objcopy
INCS = -I $(CUR_DIR)	\
		-I $(CUR_DIR)/API \
		-I $(CUR_DIR)/APP \
		-I $(CUR_DIR)/Drivers \
		-I $(CUR_DIR)/Firmware \
		-I $(CUR_DIR)/Uart0Driver \
		-I $(CUR_DIR)/uCOS_II 

SRC_DIRS = $(CUR_DIR)/API    \
		   $(CUR_DIR)/APP      \
		   $(CUR_DIR)/Drivers \
		   $(CUR_DIR)/Firmware \
		   $(CUR_DIR)/Uart0Driver \
		   $(CUR_DIR)/uCOS_II 
		
OBJS=\
		$(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC_DIRS))    


CFLAGS := -c --cpu Cortex-M3 -D__MICROLIB -g -O0 --apcs=interwork
CMACRO :=  
ASMFLAGS := --cpu Cortex-M3 -g --apcs=interwork --pd "__MICROLIB SETA 1"  
LINKFLAGS := --cpu Cortex-M3 --library_type=microlib --strict  
MAP := --autoat --summary_stderr --info summarysizes --map --xref --callgraph --symbols   
INFO := --info sizes --info totals --info unused --info veneers
    	
$(OBJ_DIR)/%.o:%.c  
    $(CC) $(CFLAGS) $(INCS) $(CMACRO) $< -o $@  
      
$(OBJ_DIR)/%.o:%.s  
    $(AS) $(ASMFLAGS) $(INCS) $(CMACRO) $< -o $@  		

	
	

all:$(OBJS)
	$(LD) $(LINKFLAGS) --libpath "$(GCC_PATH)/lib" 
--scatter start.sct $(MAP) $(INFO) --list $(LST_DIR)/$(TARGET).map $^ -o $(LST_DIR)/$(TARGET).axf 
	$(ELF) --bin -o $(BIN_DIR)/$(TARGET).bin $(LST_DIR)/$(TARGET).axf  
	$(ELF) --i32 -o $(BIN_DIR)/$(TARGET).hex $(LST_DIR)/$(TARGET).axf 

.PHONY:clean
clean:
	@rm -rf $(OBJ_DIR)
		
		
		
		
		

	