REM ### Set the environment first! -->SetEnv.cmd /x86
cl /Fo"bin\\x86\\" /EHsc /D_USRDLL /D_WINDLL /Ic:\dev\jdk7\include /Ic:\dev\jdk7\include\win32 src\NativeInput.cpp /link /DLL /OUT:bin\x86\NativeInput.x86.dll /DEFAULTLIB:User32.lib

REM /VERBOSE 
 