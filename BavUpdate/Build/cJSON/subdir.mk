################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cJSON/cJSON.c 

OBJS += \
./cJSON/cJSON.o 

C_DEPS += \
./cJSON/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
cJSON/%.o: ../cJSON/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -fPIC -I"/home/hips/karlxu/workspace/BAVUpdate/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


