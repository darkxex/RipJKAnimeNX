@echo off
rem comenta esto para desactivar el uso de la nand como root
set USENAND=true
title RipJKAnime_NX
make -j7
set a=%errorlevel%
echo ------------------------------------------

make -C RipJKForwader -j7
copy "RipJKForwader\Output\RipJKForwader[05B9DB505ABBE000][v0].nsp" "out\RipJKForwader[05B9DB505ABBE000][v0].nsp"

if %a% equ 0 color 0a
if %a% equ 0 "C:\devkitPro\tools\bin\nxlink.exe" "out\RipJKAnime_NX.nro" -p RipJKAnime_NX/RipJKAnime_NX.nro -s

echo -----------------------------------

%systemroot%\system32\timeout.exe 20
%systemroot%\system32\timeout.exe 20

