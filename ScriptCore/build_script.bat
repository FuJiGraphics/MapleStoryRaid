@echo off
:: MSBuild 전체 경로를 명시
set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: 빌드할 프로젝트 파일 설정
set PROJECT_PATH=..\Workspace\Workspace\Workspace.vcxproj

:: 출력 디렉토리 설정
set OUTPUT_DIR=Output\binbin

:: MSBuild 실행
%MSBUILD_PATH% %PROJECT_PATH% /p:Configuration=Release /p:OutDir=%OUTPUT_DIR%

:: 빌드 결과 확인
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b 1
) else (
    echo Build succeeded! Output DLL is in %OUTPUT_DIR%
)