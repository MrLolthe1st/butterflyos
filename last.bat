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
drive_add 0 if=none,id=usbstick1,file=images\q.img
device_add usb-storage,bus=ehci.0,drive=usbstick1
@echo on
call compile.bat
make run
copy binaries\startup A:\STARTUP
sync64 A:
copy images\disk.img images\boot.img
dd if=images\boot.img of=images\qqq.vhd
#qemu-system-x86_64.exe -m 32 -net nic,model=rtl8139 -drive if=none,id=usbstick,file=images\boot.img -usb -device usb-hub,bus=usb-bus.0,port=2 -device usb-storage,bus=usb-bus.0,drive=usbstick,port=2.2 -no-reboot -vga std -D aa.txt -monitor stdio 
qemu-system-x86_64.exe -m 92 -net nic,model=rtl8139   -drive if=none,id=usbstick,file=images\boot.img -usb -device usb-ehci,id=ehci -device usb-storage,bus=ehci.0,drive=usbstick -no-reboot -vga std -D aa.txt -monitor stdio -device usb-mouse 
#qemu-system-x86_64.exe -m 30 -hdc images\boot.img
#qemu-system-x86_64.exe -trace usb_host -smp cpus=2,cores=2 -m 60 -drive id=disk,file=images\boot.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0  -no-reboot -vga std -D aa.txt -monitor stdio -netdev user,id=n1,ipv6=off -device e1000,netdev=n1,mac=52:54:98:76:54:32 -usbdevice mouse
pause