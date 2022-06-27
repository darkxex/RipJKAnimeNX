@echo off
echo RipJKForwader ------------------------------------
title RipJKForwader
if not exist romfs\05B9DB505ABBE000.nsp (
	make -C RipJKForwader -j16
	move /y "RipJKForwader\Output\RipJKAnime[05B9DB505ABBE000][v0].nsp" "romfs\05B9DB505ABBE000.nsp"
)

echo RipJKAnime_NX ------------------------------------
title RipJKAnime_NX
make -j16 NSP
set a=%errorlevel%


if %a% equ 0 color 0a
"C:\devkitPro\tools\bin\nxlink.exe" "out\RipJKAnimeNX.nro"  -s

echo -----------------------------------

%systemroot%\system32\timeout.exe 20


