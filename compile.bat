@echo off
rem comenta esto para desactivar el uso de la nand como root
set USENAND=true
title RipJKAnime_NX
if not exist include\nlohmann\json.hpp (
	mkdir include\nlohmann
	wget https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp -O include/nlohmann/json.hpp
)
pause
make -j20 -C nspmini
make -j20 NSP
set a=%errorlevel%
echo ------------------------------------------
if not exist romfs\05B9DB505ABBE000.nsp (
make -C RipJKForwader -j20
copy "RipJKForwader\Output\RipJKAnime[05B9DB505ABBE000][v0].nsp" "romfs\05B9DB505ABBE000.nsp"
)

if %a% equ 0 color 0a
"C:\devkitPro\tools\bin\nxlink.exe" "out\RipJKAnime_NX.nro"  -s -a 192.168.0.7

echo -----------------------------------

%systemroot%\system32\timeout.exe 20
%systemroot%\system32\timeout.exe 20


