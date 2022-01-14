mkdir Release\zip
mkdir Release\zip\x86
mkdir Release\zip\x86\lang
mkdir Release\zip\x64
mkdir Release\zip\x64\lang

copy Release\Win32\bin\ShadowPack.exe Release\zip\x86
copy Release\Win32\bin\lang\*.dll Release\zip\x86\lang
copy Release\x64\bin\ShadowPack.exe   Release\zip\x64
copy Release\x64\bin\lang\*.dll Release\zip\x64\lang

copy ffmpeg\win32\bin\*.dll Release\zip\x86
copy ffmpeg\x64\bin\*.dll Release\zip\x64

Tools\7-Zip\7z.exe a latest.zip Release\zip\*