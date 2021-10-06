@echo off
echo RipJKForwader ------------------------------------
title RipJKForwader
if not exist romfs\05B9DB505ABBE000.nsp (
	make -C RipJKForwader -j20
	move /y "RipJKForwader\Output\RipJKAnime[05B9DB505ABBE000][v0].nsp" "romfs\05B9DB505ABBE000.nsp"
)

echo nspmini ------------------------------------------
title nspmini
git submodule update --recursive --init --remote --merge
make -j20 -C nspmini

echo RipJKAnime_NX ------------------------------------
title RipJKAnime_NX
make -j20 NSP
set a=%errorlevel%


if %a% equ 0 color 0a
"C:\devkitPro\tools\bin\nxlink.exe" "out\RipJKAnimeNX.nro"  -s -a 192.168.0.7

echo -----------------------------------

%systemroot%\system32\timeout.exe 20
%systemroot%\system32\timeout.exe 20


