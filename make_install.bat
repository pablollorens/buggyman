@echo off
xcopy /S/E/K bin\*.* _install\
"%PROGRAMFILES%\NSIS\makensis.exe" "/XSetCompressor /SOLID /FINAL lzma" Buggy-Man.nsi
rmdir /S/Q _install

