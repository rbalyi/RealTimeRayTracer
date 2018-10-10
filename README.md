# RealTimeRayTracer
Simple meshless real-time ray tracer in C++ using OpenCL and OpenGL.

The bin folder contains the Win10 64 bit executable.
The opengllibs has all the used OpenGL libraries.
It uses the Intel OpenCL SDK and tested on Intel(R) HD Graphics 630 (30 FPS) and Intel(R) HD Graphics 4400 (12 FPS) integrated GPUs.

Example setup for Visual Studio (orders matter)
All Configurations / All Platforms
VC++ Directories
	-> Include Directories: C:\opengllibs\FreeType2.6.3;C:\opengllibs\GLEW2.0\include;C:\opengllibs\GLFW3.2.1\include;C:\opengllibs\GLM0.9.7.3;C:\opengllibs\SOIL\include;$(INTELOCLSDKROOT)/include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
	-> 	Library Directories: C:\opengllibs\FreeType2.6.3\lib;C:\opengllibs\GLEW2.0\lib\Release\x64;C:\opengllibs\GLFW3.2.1\lib-vc2015;C:\opengllibs\SOIL\lib;$(INTELOCLSDKROOT)/lib/x64;<different options>

Linker
	-> Input
		-> Additional Dependencies: opengl32.lib;freetype263_64bit.lib;glu32.lib;glew32s.lib;glfw3.lib;SOIL.lib;OpenCL.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)

C/C++
	-> Preprocessor
		-> Preprocessor Definitions: GLEW_STATIC;_MBCS;%(PreprocessorDefinitions)

Platform: x64
All resource files are searched in the ..\data folder relative to the executable.

https://www.youtube.com/watch?v=Z36RvvSb72Q&feature=youtu.be
