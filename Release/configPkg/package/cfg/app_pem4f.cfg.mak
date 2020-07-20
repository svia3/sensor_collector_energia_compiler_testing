# invoke SourceDir generated makefile for app.pem4f
app.pem4f: .libraries,app.pem4f
.libraries,app.pem4f: package/cfg/app_pem4f.xdl
	$(MAKE) -f C:\Users\a0488612\workspace_v10\sensor_collector_energia_ti_compiler_testing\sensor_collector_energia_ti_compiler_testing/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\a0488612\workspace_v10\sensor_collector_energia_ti_compiler_testing\sensor_collector_energia_ti_compiler_testing/src/makefile.libs clean

