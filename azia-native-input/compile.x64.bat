REM ### Set the environment first! -->SetEnv.cmd /x64
cl /Fo"bin\\x64\\" /EHsc /D_USRDLL /D_WINDLL /Ic:\dev\jdk7\include /Ic:\dev\jdk7\include\win32 src\NativeInput.cpp /link /DLL /OUT:bin\x64\NativeInput.x64.dll /DEFAULTLIB:User32.lib

REM /VERBOSE 
