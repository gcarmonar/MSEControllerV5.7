################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
pkg/comm/comm.obj: ../pkg/comm/comm.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.4/bin/cl2000" -v28 -ml -mt --float_support=softlib --include_path="c:/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.4/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/idle" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/f2833x/v140/DSP2833x_headers/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/f2833x/v140/DSP2833x_common/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/comm" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/conin" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/conout" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/scon" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/zcon" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/os" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3" --advice:performance=all -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="pkg/comm/comm.pp" --obj_directory="pkg/comm" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pkg/comm/uart.obj: ../pkg/comm/uart.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.4/bin/cl2000" -v28 -ml -mt --float_support=softlib --include_path="c:/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.4/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/idle" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/f2833x/v140/DSP2833x_headers/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/f2833x/v140/DSP2833x_common/include" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/comm" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/conin" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/conout" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/scon" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/zcon" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3/pkg/os" --include_path="C:/Users/JALILFRANCISCO/Documents/Maestria/5_semestre/MSEControllerV5.3" --advice:performance=all -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="pkg/comm/uart.pp" --obj_directory="pkg/comm" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


