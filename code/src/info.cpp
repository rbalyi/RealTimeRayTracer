#include "../header/info.h"

using namespace std;

namespace Info {

	void PrintError(const cl_int error, const char* functionName) {
		if (CL_SUCCESS != error) {
			cout << "\nError calling " << functionName << " Error code: " << error << endl;
		}
	}

	void PrintDeviceInfo(cl_device_id device) {
		size_t infoSize = 0;
		char* info = nullptr;
		int ids[] = { CL_DEVICE_NAME, CL_DEVICE_VENDOR };
		const char* pTexts[] = { "\nDevice name: \t", "\nDevice vendor: \t"  };
		int textIndex = 0;
		for (auto id : ids) {
			PrintError(clGetDeviceInfo(device, id, 0, nullptr, &infoSize), "clGetDeviceInfo");
			info = (char*)malloc(infoSize);
			PrintError(clGetDeviceInfo(device, id, infoSize, info, nullptr), "clGetDeviceInfo");
			cout << pTexts[textIndex++] << info;
			free(info);
			info = nullptr;
			infoSize = 0;
		}
	}

	void PrintPlatformInfo(const cl_platform_id platform) {
		size_t infoSize = 0;
		char* info = nullptr;
		int ids[] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR };
		const char* pTexts[] = { "\nPlatform name: \t", "\nPlatform vendor: \t" };
		int textIndex = 0;
		for (auto id : ids) {
			PrintError(clGetPlatformInfo(platform, id, 0, nullptr, &infoSize), "clGetPlatformInfo");
			info = (char*)malloc(infoSize);
			PrintError(clGetPlatformInfo(platform, id, infoSize, info, nullptr), "clGetPlatformInfo");
			cout << pTexts[textIndex++] << info;
			free(info);
			info = nullptr;
			infoSize = 0;
		}
	}

}