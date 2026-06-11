@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cl /EHsc /std:c++17 /MD /I"include" /I"C:\Users\User\miniconda3\Library\include" ^
   src\shared\CarValidator.cpp ^
   test\test_main.cpp ^
   /Fe:"test_runner.exe" ^
   /link "C:\Users\User\miniconda3\Library\lib\gmock.lib" "C:\Users\User\miniconda3\Library\lib\gmock_main.lib"
