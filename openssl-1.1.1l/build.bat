perl Configure --prefix=C:\Data\shadowpack\openssl\win32\share --openssldir=C:\Data\shadowpack\openssl\win32\share VC-WIN32
nmake
nmake test
nmake install
nmake distclean


perl Configure --prefix=C:\Data\shadowpack\openssl\win32\share_debug --openssldir=C:\Data\shadowpack\openssl\win32\share_debug --debug VC-WIN32
nmake
nmake test
nmake install
nmake distclean


perl Configure --prefix=C:\Data\shadowpack\openssl\win32\static --openssldir=C:\Data\shadowpack\openssl\win32\static no-shared VC-WIN32
nmake
nmake test
nmake install
nmake distclean

perl Configure --prefix=C:\Data\shadowpack\openssl\win32\static_debug --openssldir=C:\Data\shadowpack\openssl\win32\static_debug --debug no-shared VC-WIN32
nmake
nmake test
nmake install
nmake distclean



perl Configure --prefix=C:\Data\shadowpack\openssl\win64\share --openssldir=C:\Data\shadowpack\openssl\win64\share VC-WIN64A-masm
nmake
nmake test
nmake install
nmake distclean

perl Configure --prefix=C:\Data\shadowpack\openssl\win64\share_debug --openssldir=C:\Data\shadowpack\openssl\win64\share_debug --debug VC-WIN64A-masm
nmake
nmake test
nmake install
nmake distclean


perl Configure --prefix=C:\Data\shadowpack\openssl\win64\share --openssldir=C:\Data\shadowpack\openssl\win64\static no-shared VC-WIN64A-masm
nmake
nmake test
nmake install
nmake distclean

perl Configure --prefix=C:\Data\shadowpack\openssl\win64\share_debug --openssldir=C:\Data\shadowpack\openssl\win64\static_debug --debug no-shared VC-WIN64A-masm
nmake
nmake test
nmake install
nmake distclean
