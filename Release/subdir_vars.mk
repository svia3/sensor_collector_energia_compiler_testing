################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../app.cfg 

CMD_SRCS += \
../cc13x2lp.cmd 

SYSCFG_SRCS += \
../collector.syscfg 

C_SRCS += \
../CC1312R1_LAUNCHXL_fxns.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./configPkg/ \
./syscfg/ 

C_DEPS += \
./CC1312R1_LAUNCHXL_fxns.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_radio_config.d \
./syscfg/ti_drivers_config.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./CC1312R1_LAUNCHXL_fxns.obj \
./syscfg/ti_devices_config.obj \
./syscfg/ti_radio_config.obj \
./syscfg/ti_drivers_config.obj 

GEN_MISC_FILES += \
./syscfg/ti_154stack_config.h \
./syscfg/ti_154stack_features.h \
./syscfg/ti_radio_config.h \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.exp \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" \
"syscfg\" 

OBJS__QUOTED += \
"CC1312R1_LAUNCHXL_fxns.obj" \
"syscfg\ti_devices_config.obj" \
"syscfg\ti_radio_config.obj" \
"syscfg\ti_drivers_config.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_154stack_config.h" \
"syscfg\ti_154stack_features.h" \
"syscfg\ti_radio_config.h" \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.exp" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"CC1312R1_LAUNCHXL_fxns.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_radio_config.d" \
"syscfg\ti_drivers_config.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" \
"syscfg\ti_devices_config.c" \
"syscfg\ti_radio_config.c" \
"syscfg\ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../CC1312R1_LAUNCHXL_fxns.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_radio_config.c" \
"./syscfg/ti_drivers_config.c" 

SYSCFG_SRCS__QUOTED += \
"../collector.syscfg" 


