@echo off

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
    IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    color 0A
    echo First-build configator.
    echo If Windows has opened it window twice, just close it now, and run it with admin rights.
    pause
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params= %*
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params:"=""%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:-------------------------------------- 
md binaries >nul
md images >nul
cd binaries >nul
md input >nul
md memory >nul
md std >nul
md storage >nul
md usb >nul
cd ..
imdisk -d -m A: >nul
dd if=/dev/zero of=images/disk.img bs=1k count=71000 >nul
imdisk -a -f images/disk.img -m A:
if '%errorlevel%' NEQ '0' (
    echo Install ImDisk and continue
    pause
    exit
)
format /FS:FAT32 A: /y
sync64 A:
imdisk -d -m A:
nasm sources/bootloader.asm -o images/bootloader
cd images
g++ ../rewriteboot.cpp -std=c++11 -o makeboot.exe
makeboot
copy c.img disk.img
del /f c.img
del /f makeboot.exe
del /f bootloader
imdisk -a -f disk.img -m A:
echo Configured.
pause