################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/xsysmon_psu/subdir.mk
-include src/xsysmon/subdir.mk
-include src/peripheral/subdir.mk
-include src/interrupts/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
ZCU102trigger.elf.size \


# All Target
all: ZCU102trigger.elf secondary-outputs

# Tool invocations
ZCU102trigger.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM v8 gcc linker'
	aarch64-none-elf-gcc -Wl,-T -Wl,../src/lscript.ld -L../../ZCU102trigger_bsp/psu_cortexa53_1/lib -o "ZCU102trigger.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ZCU102trigger.elf.size: ZCU102trigger.elf
	@echo 'Invoking: ARM v8 Print Size'
	aarch64-none-elf-size ZCU102trigger.elf  |tee "ZCU102trigger.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) ZCU102trigger.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
