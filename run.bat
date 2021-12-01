@echo off

rem run with argument "vs" if you want to start visual studio to debug

IF "%1"=="vs" (
    devenv cmake-build-debug\ADK.exe
) ELSE (
    pushd cmake-build-debug
    START /D ..\data ADK.exe
    popd
)