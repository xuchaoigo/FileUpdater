################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../updater/updater.cpp 

OBJS += \
./updater/updater.o 

CPP_DEPS += \
./updater/updater.d 


# Each subdirectory must supply rules for building sources it contributes
updater/%.o: ../updater/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fPIC -I"/home/hips/karlxu/workspace/BAVUpdate/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


