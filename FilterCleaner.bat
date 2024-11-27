@echo off
REM 현재 디렉토리와 하위 디렉토리에서 .vcxproj, .vcxproj.filters, .vcxproj.user 파일 삭제

echo Deleting .vcxproj, .vcxproj.filters, and .vcxproj.user files...

REM 현재 디렉토리에서 파일 삭제
del /s /q *.vcxproj
del /s /q *.vcxproj.filters
del /s /q *.vcxproj.user

echo Files deleted successfully.
pause