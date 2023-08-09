################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/helloworld.c \
../src/platform.c 

OBJS += \
./src/helloworld.o \
./src/platform.o 

C_DEPS += \
./src/helloworld.d \
./src/platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -I../../triggerforaes_bsp/psu_cortexa53_1/include -I/home/manvar00/Downloads/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/src -I/home/manvar00/Downloads/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/src/xsysmon_psu -I/home/manvar00/Downloads/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/src/peripheral -I/home/manvar00/Downloads/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/src/xsysmon -I/home/manvar00/Downloads/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/src/interrupts -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


