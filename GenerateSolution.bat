@echo off

echo Deleting .vcxproj, .vcxproj.filters, and .vcxproj.user files...

del /s /q *.vcxproj
del /s /q *.vcxproj.filters
del /s /q *.vcxproj.user

CALL vendor\premake5\bin\premake5.exe vs2022
PAUSE