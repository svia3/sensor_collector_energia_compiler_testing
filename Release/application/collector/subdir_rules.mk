################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
application/collector/%.obj: ../application/collector/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" --cmd_file="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/application/defines/collector.opts"  -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=5 --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/Release" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/application/collector" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/application" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/services" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/hal/crypto" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/hal/platform" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/hal/rf" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/mac/high_level" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/mac/low_level" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/software_stack/ti15_4stack/osal_port" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/apps" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/boards" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/osal_port" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/util" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/inc" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/stack/src" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/stack/tirtos/inc" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/common/heapmgr" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/services/saddr" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/services/sdata" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/hal/crypto" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/hal/platform" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/hal/rf" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/fh" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/high_level" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/inc" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/rom" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/inc/cc13xx" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/low_level/cc13xx" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/tracer" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/ti154stack/apps/collector" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/devices/cc13x2_cc26x2" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/devices/cc13x2_cc26x2/inc" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/devices/cc13x2_cc26x2/driverlib" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/common/nv" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/common/cui" --include_path="/Users/robertinant/ti/simplelink_cc13x2_26x2_sdk_4_10_00_78/source/ti/posix/ccs" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=TIMAC_ROM_IMAGE_BUILD --define=Board_EXCLUDE_NVS_EXTERNAL_FLASH --define=DeviceFamily_CC13X2 -g --c99 --plain_char=unsigned --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --embedded_constants=on --unaligned_access=on --enum_type=packed --wchar_t=16 --common=on --fp_reassoc=off --sat_reassoc=off --preproc_with_compile --preproc_dependency="application/collector/$(basename $(<F)).d_raw" --include_path="/Users/robertinant/ccs_workspaces/test/collector_energia_ti_compiler_bak/Release/syscfg" --obj_directory="application/collector" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

