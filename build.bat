@echo off
cd "%~dp0"
g++ -mwindows -s -Os WinMain.cpp -lole32 -lwinmm -o WinVolumeHotKeys.exe
upx --best --ultra-brute WinVolumeHotKeys.exe>nul 2>&1