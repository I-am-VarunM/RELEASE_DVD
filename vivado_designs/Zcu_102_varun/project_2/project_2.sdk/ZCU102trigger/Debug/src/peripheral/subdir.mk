################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/peripheral/i2c.c \
../src/peripheral/pmodtmp2.c \
../src/peripheral/system_test.c \
../src/peripheral/xgpio_tapp_example.c \
../src/peripheral/xuartps_intr_example.c 

OBJS += \
./src/peripheral/i2c.o \
./src/peripheral/pmodtmp2.o \
./src/peripheral/system_test.o \
./src/peripheral/xgpio_tapp_example.o \
./src/peripheral/xuartps_intr_example.o 

C_DEPS += \
./src/peripheral/i2c.d \
./src/peripheral/pmodtmp2.d \
./src/peripheral/system_test.d \
./src/peripheral/xgpio_tapp_example.d \
./src/peripheral/xuartps_intr_example.d 


# Each subdirectory must supply rules for building sources it contributes
src/peripheral/%.o: ../src/peripheral/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -I../../ZCU102trigger_bsp/psu_cortexa53_1/include -I/home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon/src/classifier/ -I/home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon/src/interrupts/ -I/home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon/src/peripheral/ -I/home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon/src/xsysmon/ -I/home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon/src/xsysmon_psu/ -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


