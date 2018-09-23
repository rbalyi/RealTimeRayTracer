#pragma once

#include <CL/cl.h>
#include <iostream>

namespace Info {
	
	void PrintDeviceInfo(cl_device_id device);
	void PrintPlatformInfo(const cl_platform_id platform);
	void PrintError(const cl_int error, const char* functionName);

}