@RD /S /Q "C:\respaldo2017\RipJKAnimeNX\build"
xcopy "C:\respaldo2017\C++\test\test\*.cpp" "C:\respaldo2017\RipJKAnimeNX\source\*.cpp" /y
make
nxlink "out/RipJKAnime_NX.nro"