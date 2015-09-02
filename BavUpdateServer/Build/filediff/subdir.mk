################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../filediff/filediff.cpp 

OBJS += \
./filediff/filediff.o 

CPP_DEPS += \
./filediff/filediff.d 


# Each subdirectory must supply rules for building sources it contributes
filediff/%.o: ../filediff/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fPIC -I"/home/hips/karlxu/workspace/BavUpdateServer/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


