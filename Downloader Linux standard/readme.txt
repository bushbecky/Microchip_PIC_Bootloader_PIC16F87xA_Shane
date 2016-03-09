From: "veryuniqueuserid" <veryuniqueuserid@yahoo.com>
Date: Wed Sep 7, 2005  12:13 pm
Subject: Bootloader w/ Modtronix DEVKIT44B and Linux

I had a little trouble getting the bootloader to work with the Modtronix
DEVKIT44B using a Linux machine as the host. The bootloader code for Linux is built with
hardware handshaking (CRTSCTS in linuxio.c:cominit()). I disabled this and it
now works fine.

Thank-you for a great tool.

Ed