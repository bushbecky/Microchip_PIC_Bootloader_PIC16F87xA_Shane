Hi,

I’m a hobbist and I actually use your bootloader for PIC16F877.

I’m offering you a Binary version and a patch of your Downloader for MIPSEL 
processor.

This binary can be used on any board running OpenWRT http://www.openwrt.org/.

This is very useful as you can make a fully autonomous system, you can flash 
your PIC without any serial stuff connected to your personal computer.

(if you add a couple of stuff in your program your board can reboot 
automatically when it detect the Downloader)

Regards,
Brice GIBOUDEAU

PS : If you are interested by the MIPS release of the “PIC16 Downloader” don’t 
hesitate to indicate me a way to send it to you.

Hi,

I Build it on a "Debian Etch x32" with a MIPS cross-compiler.
You can find below what I change to your source code and attached to the mail the file dlpic-mips.tar.gz which contain the modified source code and the binary version.

Regards,

---------------

mips-linux:/usr/src# diff dlpic-orig dlpic-mips
diff dlpic-orig/linuxio.c dlpic-mips/linuxio.c
79,80c79
<       CRTSCTS | /* hardware flow control */
<       CS8 | CSTOPB | /* 8 bits, no parity, 2 stop bits */
---
>       CS8 | /* 8 bits, no parity, 1 stop bits CSTOPB */
diff dlpic-orig/Makefile dlpic-mips/Makefile
5a6,9
> CC=mipsel-linux-gnu-gcc
> CFLAGS= -mips32 -static
> LDFLAGS=
>
Binary files dlpic-orig/picdl and dlpic-mips/picdl differ

---------------

mips-linux:~# mipsel-linux-gnu-gcc -v
Using built-in specs.
Target: mipsel-linux-gnu
Configured with: ../src/configure -v --enable-languages=c,c++ --prefix=/usr --enable-shared --with-system-zlib --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --enable-nls --with-gxx-include-dir=/usr/mipsel-linux-gnu/include/c++/4.1.2 --program-suffix=-4.1 --enable-__cxa_atexit --enable-clocale=gnu --enable-libstdcxx-debug --disable-libssp --enable-checking=release --program-prefix=mipsel-linux-gnu- --includedir=/usr/mipsel-linux-gnu/include --build=i486-linux-gnu --host=i486-linux-gnu --target=mipsel-linux-gnu
Thread model: posix
gcc version 4.1.2 20061115 (prerelease) (Debian 4.1.1-21)