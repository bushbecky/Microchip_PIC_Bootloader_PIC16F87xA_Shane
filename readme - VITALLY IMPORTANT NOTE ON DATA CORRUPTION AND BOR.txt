Received the following email on 2005-08-27

VERY IMPORTANT - READ THIS BEFORE BURNING BOOTLOADER - FAILURE TO SET THE BROWN
OUT RESET BIT WILL RESULT IN THE PIC POSSIBLY DYING!! THE BROWN OUT RESET BIT
(BOR) IS CURRENTLY *NOT* SET IN THE ASSEMBLY SOURCE, AS IT EASES FIRST TIME
BRINGUP OF THE BOOTLOADER.

Dear Shane,

Although I'm still very positive about the PIC16F877 boatloader I have recently
expierenced a very nasty side effect I think others should know about it before
they start using it for their rocket detonators, breathing devices etc.

At the research institute where I work, I have added the bootloader to the
firmware of about a 100 sensors. This has proven to be very handy for field
upgrading, changing calibration tables etc.  But after a while sensors started
coming back. When I readback the code, I noticed a few bits of code had changed.
Very scary !!!

So I spend some of time finding the cause. I made a little 'spurious write
detection' program that compares the lower half of the code memory with the
upper half where I put a mirror of the code. When I programmed this into a
PIC16F877 (using ICD2) and connected the device to a 'very very dirty power
supply' I carried out the following experiments:

- with a bootloader, Brownout Detect (BOR)=off, Power Up Timer=off, LVP=on:
spurious writes after a just few power interrupts;

- without bootloader, same config setting: no problem after many power
interrupts;

- with bootloader, with BOR on: also no problems after many power interrupts;

Conclusion: never use this bootloader with the BOR switched off!

I think you should put this warning very loud and clear to all the user of this
bootloader.

Kind Regards,

Herman Aartsen.
