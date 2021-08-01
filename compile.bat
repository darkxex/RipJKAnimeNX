@echo off
rem comenta esto para desactivar el uso de la nand como root
set USENAND=true
set EXPORTNSP=true
title RipJKAnime_NX
del "out\*.nsp"
make -j20
set a=%errorlevel%
echo ------------------------------------------

make -C RipJKForwader -j20
copy "RipJKForwader\Output\RipJKForwader[05B9DB505ABBE000][v0].nsp" "out\RipJKForwader[05B9DB505ABBE000][v0].nsp"

if %a% equ 0 color 0a
"C:\devkitPro\tools\bin\nxlink.exe" "out\RipJKAnime_NX.nro"  -s -a 192.168.0.7

echo -----------------------------------

%systemroot%\system32\timeout.exe 20
%systemroot%\system32\timeout.exe 20


