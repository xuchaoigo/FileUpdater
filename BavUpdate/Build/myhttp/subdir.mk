################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../myhttp/MyHttp.cpp 

OBJS += \
./myhttp/MyHttp.o 

CPP_DEPS += \
./myhttp/MyHttp.d 


# Each subdirectory must supply rules for building sources it contributes
myhttp/%.o: ../myhttp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fPIC -I"/home/hips/karlxu/workspace/BAVUpdate/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


