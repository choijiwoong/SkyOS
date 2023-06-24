REM Start qemu on windows.
@ECHO OFF
SET SDL_VIDEODRIVER=windlib
SET SDL_AUDIODRIVER=dsound
SET QEMU_AUDIO_DRV=dsound
SET QEMU_AUDIO_LOG_TO_MONITOR=0
qemu-system-x86_62.exe -L . -m 128 -fda grub.ima -soundhw sb16,es1370 -localtime -M pc