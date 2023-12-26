@echo off
cd %~dp0
cd ..
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
code --new-window . | exit