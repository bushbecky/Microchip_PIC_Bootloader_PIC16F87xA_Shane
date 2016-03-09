;******************************************************************************
;   Bootloader for Microchip PIC16F870/871/873/874/876A/877A                  *
;                     (c) 2000-2002, Petr Kolomaznik                          *
;                          Freely distributable                               *
;******************************************************************************
;                                                                             *
; Original Author:   Petr Kolomaznik                                          *
; Company:           EHL elektronika, Czech Republic                          *
; Email:             kolomaznik@ehl.cz                                        *
; Url:               www.ehl.cz/pic                                           *
; Modified by:       Shane Tolmie                                             *
; Company:           DesignREM                                                *
; Email:             shane.tolmie@microchipc.com                              *
; Url:               www.microchipc.com                                       *
; Modified by:       Peter Huemer                                             *
; Company:           HTBLA Steyr, Abt. Elektronik / Techn. Informatik         *
; Email:             p.huemer@eduhi.at                                        *
; Url:               www.htl-steyr.ac.at                                      *
;                                                                             *
;******************************************************************************
; How to use this bootloader:                                                 *
; - open project in MPLAB                                                     *
; - check and modify of parameters in the user setting section with <<< mark  *
; - make project with MPLAB and MPASM                                         *
; - use any programmer for programming of bootldr.hex to microcontroller      *
; - set configuration bits                                                    *
; - use PIC downloader program for user program download                      *
; - check if user program doesn't use the top 214 program memory locations    *
;                                                                             *
; Notes:                                                                      *
; - tab size for editor is 2                                                  *
;******************************************************************************
; 2.81 - 24.12.2005 (Shane Tolmie [shane@microchipc.com and Nozomu Muto       *
;   re8n-mtu@asahi-net.or.jp. See version readme file.                        *
; 2.80 - 27.08.2005 (Shane Tolmie [shane@microchipc.com and Herman Aartsen    *
;    h.aartsen@chello.nl). Added warning note about BOR and dirty power       *
;    supplies leading to the PIC dying.                                       *
; 2.70 - 24.09.2004 (Shane Tolmie [shane@microchipc.com and Peter Huemer      *
;    p.huemer@eduhi.at). Fixed quad-alignment problem with 16F87xA devices.   *
;	 The code used to fail unless the .hex file had a length that wasn't a    *
;    multiple  of 4.                                                          *
; 2.60 - 29.12.2002 (Shane Tolmie [shane@microchipc.com])                     *
;  - made it so that frequencies <=4Mhz have XT, and freq >4Mhz have HS       *
;    configuration in the fuse bits, to enable it to work at all freq with    *
;    resonators.                                                              *
; 2.50 - 28.8.2002 (Shane Tolmie [shane@microchipc.com])                      *
;  - switched off BODEN which resets part if low voltage power supply used    *
;    (many grateful thanks to Richard (joshanks@earthlink.net)                *
; 2.50 - 28.8.2002 (Shane Tolmie [shane@microchipc.com])                      *
;  - switched off BODEN which resets part if low voltage power supply used    *
;    (many grateful thanks to Richard (joshanks@earthlink.net)                *
; 2.20 - 8.8.2001 (Shane Tolmie [shane@microchipc.com])                       *
;  - added watchdog user change, defaults to off (leaving it on can create    *
;    difficult to track down problems)                                        *
; 2.10 - 3.8.2001 (jvo@vinylium.ch)                                           *
;  - substantially reduced size of program                                    *
;  - restores USART to reset condition before starting user program           *
;  - made user program start variable                                         *
;  - added trap to avoid unintended running into bootloader                   *
;  - watchdog timeout is directly handled over to user program                *
; 2.00 - 30.03.2001 (Shane Tolmie [shane@microchipc.com])                     *
;  - for the 16F876, the 4 instructions in the original .hex file at 0x0000   *
;    to 0x0003 are copied to 0x1F00 to 0x1F03, and executed once the          *
;    bootloader is finished, to jump back to user code. The reset vector in   *
;    the chip at 0x0000 to 0x0003 points to the bootloader.  Inserted a       *
;    pagesel 0x0000 (2 instructions) at 0x1F00 to make a short jump into a    *
;    long jump so it would work with .hex files that didnt have a long jump   *
;    as the first 4 instructions in the .hex file.                            *
;    This addition dramatically increases compatibility with .hex files.      *
; 1.06 - 30.03.2001 (Shane Tolmie)                                            *
;  - added config bits                                                        *
;  - program now jumps immediately to user program after download             *
; 1.02 - 15.11.2000                                                           *
;  - added check of user constants                                            *
;  - added support for the new 16F870/1/2                                     *
;  - added errorlevel directive for message 302 and 306                       *
;******************************************************************************

	;;;;;;;;;vvv 2005-12-22mn
	#define  PICC_LITE_950
	;;;;;;;;;^^^ 2005-12-22mn

	errorlevel -302, -305, -306		; no message 302 and 306
	list       b=2			; tabulator size = 2

;================== User setting section ======================================

	list p=16f877a			; <<< set type of microcontroller  (16f873a or 16f876a)
;     set same microcontroller in the project
	#define ICD_DEBUG 0		; <<< if using MPLAB ICD Debugger, moves bootloader down 256 bytes to make room for it [0|1]
	#define FOSC D'20000000' 	; <<< set quartz frequence [Hz], max. 20 MHz
	;#define BAUD D'38400'		; <<< set baud rate [bit/sec]
	#define BAUD D'19200'		; <<< set baud rate [bit/sec]
	#define	BAUD_ERROR	D'4'	;	<<< set baud rate error [%]
	#define TIME			; <<< set method of bootloader start PIN/TIME
					;     PIN	: start on low level of trigger pin
                            		;     TIME: start on receive IDENT byte in TIMEOUT
    #define	TRIGGER		PORTB,7 ; <<< only for PIN - set PORT_X,PIN_NR
	#define	TIMEOUT		D'2'	; <<< only for TIME - set time [0.1s], max. 25 sec
	#define WATCHDOGTIMER 0		; <<< Watchdog timer default OFF/ON [0|1]

;=================== Configuration ============================================

	__IDLOCS H'2100'		; version ID of bootloader

  IF WATCHDOGTIMER == 0
    #define MY_WDT _WDT_OFF
  ELSE
    #define MY_WDT _WDT_ON
  ENDIF

  IF FOSC<=D'4000000'
    #define _MYCRYSTAL _XT_OSC ;see datasheet
  ELSE
    #define _MYCRYSTAL _HS_OSC
  ENDIF

  ;note: for high voltage parts, you can set BODEN_ON, but for low voltage parts
  ;it resets the circuit continuously!

  ;NOTE: IF AT ALL POSSIBLE, SET BROWN OUT DETECT TO 'ON' TO AVOID DIRTY POWER
  ;SUPPLIES KILLING THE PIC MEMORY

	;--start email from Herman Aartsen--
	;Dear Shane,
	;
	;Although I'm still very positive about the PIC16F877 boatloader I have
	;recently expierenced a very nasty side effect I think others should know about
	;it before they start using it for their rocket detonators, breathing devices
	;etc.
	;
	;At the research institute where I work, I have added the bootloader to the
	;firmware of about a 100 sensors. This has proven to be very handy for field
	;upgrading, changing calibration tables etc.  But after a while sensors started
	;coming back. When I readback the code, I noticed a few bits of code had
	;changed. Very scary !!!
	;
	;So I spend some of time finding the cause. I made a little 'spurious write
	;detection' program that compares the lower half of the code memory with the
	;upper half where I put a mirror of the code. When I programmed this into a
	;PIC16F877 (using ICD2) and connected the device to a 'very very dirty power
	;supply' I carried out the following experiments:
	;
	;- with a bootloader, Brownout Detect (BOR)=off, Power Up Timer=off, LVP=on:
	;spurious writes after a just few power interrupts;
	;
	;- without bootloader, same config setting: no problem after many power
	;interrupts;
	;
	;- with bootloader, with BOR on: also no problems after many power interrupts;
	;
	;Conclusion: never use this bootloader with the BOR switched off!
	;
	;I think you should put this warning very loud and clear to all the user of
	;this bootloader.
	;
	;Kind Regards,
	;
	;Herman Aartsen.
	;
	;Note: most user would not even get the bootloader working if the BOR was
	;enabled by default turning on the BOR makes the bootloader inoperable with
	;low voltage PIC's. For this reason, and others (including the inability of a
	;PIC18F2550 to erase its code protection bits unless the voltage is 5V) I would
	;highly recommend running your PIC at 5V and turning the BOR bit on
	;
	;Shane Tolmie (www.microchipc.com)
	;
	;--end email from Herman Aartsen--

  __CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_ON & _MYCRYSTAL & _WRT_OFF & _LVP_OFF & _DEBUG_OFF & _CPD_OFF

;=============== End of user setting section ==================================


;================== Check User Constants ======================================

  IFNDEF FOSC
    ERROR "FOSC must be defined"
  ENDIF

  IFNDEF BAUD
    ERROR "BAUD must be defined"
  ENDIF

	IF FOSC > D'20000000'
		ERROR "max. quartz frequency = 20 MHz"
	ENDIF

	IFNDEF PIN
		IFNDEF TIME
			ERROR "wrong start method of bootloader, must be PIN or TIME"
		ENDIF
	ENDIF

	IF TIMEOUT > D'254'
		ERROR "max. timeout = 25.4 sec"
	ENDIF

	IF ICD_DEBUG != 0
		IF ICD_DEBUG != 1
			ERROR "ICD debug must be 1 (enabled) or 0 (not used)"
		ENDIF
	ENDIF

;========================== Constants =========================================

	IF ((FOSC/(D'16' * BAUD))-1) < D'256'
		#define DIVIDER (FOSC/(D'16' * BAUD))-1
		#define HIGH_SPEED 1
	ELSE
		#define DIVIDER (FOSC/(D'64' * BAUD))-1
		#define HIGH_SPEED 0
	ENDIF

BAUD_REAL	EQU	FOSC/((D'64'-(HIGH_SPEED*D'48'))*(DIVIDER+1))

	IF BAUD_REAL > BAUD
		IF (((BAUD_REAL - BAUD)*D'100')/BAUD) > BAUD_ERROR
			ERROR	"wrong baud rate"
		ENDIF
	ELSE
		IF (((BAUD - BAUD_REAL)*D'100')/BAUD) > BAUD_ERROR
			ERROR	"wrong baud rate"
		ENDIF
	ENDIF

	IF FOSC > D'10240000'
		#define	T1PS 8
		#define	T1SU 0x31
	ELSE
		IF FOSC > D'5120000'
			#define T1PS 4
			#define T1SU 0x21
		ELSE
			IF FOSC > D'2560000'
				#define T1PS 2
				#define T1SU 0x11
			ELSE
				#define T1PS 1
				#define T1SU 0x01
			ENDIF
		ENDIF
	ENDIF

TIMER	EQU	(D'65538'-(FOSC/(D'10'*4*T1PS))); reload value for TIMER1 (0.1s int)

;>>> ADAPT
	IFDEF __16F873A
	  	#include <p16f873a.inc>
	  	#define ProgHI 0x0FFF
  	ELSE
		IFDEF __16F876A
	    	#include <p16f876a.inc>
      		#define ProgHI 0x1FFF
    	ELSE
			IFDEF __16F877A
	    		#include <p16f877a.inc>
      			#define ProgHI 0x1FFF
    		ELSE
        		ERROR   "incorrect type of microcontroller"
    		ENDIF
    	ENDIF
    ENDIF
;>>>

;>>> ADAPT
#define LoaderSize	0x180  		                ; size of bootloader (not yet optimized)
;>>>

; #define LoaderMain	UserStart+5			; main address of bootloader
#define LoaderTop 	ProgHI-(ICD_DEBUG*0x100)        ; top address of bootloader
#define LoaderStart	(LoaderTop)-LoaderSize+1        ; start address of bootloader

#define	NumRetries	1				; number of writing retries


#define WRITE       0xE3				; communication protocol
#define WR_OK       0xE4
#define WR_BAD      0xE5

#define DATA_OK     0xE7
#define DATA_BAD    0xE8

#define IDENT       0xEA
#define IDACK       0xEB

#define DONE        0xED

;=============== Variables ====================================================

buff			EQU	0x20
; RAM address 0x70 reserved for MPLAB-ICD
amount			EQU	0x71
chk1			EQU	0x72
chk2			EQU	0x73
retry			EQU	0x74
address			EQU     0x75
tmpaddr			EQU     0x77
temp			EQU	0x79
time			EQU	0x7A
count			EQU     0x7B
;>>> ADAPT
help                    EQU     0x7C
lcount			EQU	0x7d
;>>>

;------------------------------------------------------------------------------
    ORG     0x0000                                      ; reset vector of microcontroller
	nop						; for compatibility with ICD
	pagesel Main
    goto    Main

;------------------------------------------------------------------------------
	ORG		LoaderStart
TrapError
	pagesel TrapError
	goto	TrapError				; trap for unintended running into Bootloader

UserStart
	; this instruction never gets overwritten
	clrf	PCLATH					; set PCLATH to program page 0
;>>> ADAPT
	IFDEF __16F873A
	nop                                             ; for quad alignment
	ENDIF
;>>>
	; the following 4 instructions get overwritten by user program
	pagesel UserStart				; set PCLATH to program page of UserStart
	goto	UserStart				; loop for first start without a user program
;>>> ADAPT
	nop
	IFDEF __16F873A
	nop
	ENDIF
;>>>

;;;;;;;;;vvv 2005-12-22mn
	IFDEF PICC_LITE_950
        nop
        nop
        nop
        nop
        ENDIF
;;;;;;;;;^^^ 2005-12-22mn

;------------------------------------------------------------------------------
Main
	btfss STATUS,NOT_TO				; run user program after WatchDog-TimeOut
	goto UserStart
start
	movlw	0x90					; SPEN = 1, CREN = 1
	movwf	RCSTA
	bsf	STATUS,RP0				; bank1
	IF HIGH_SPEED == 1				; USART SYNC=0; SPEN=1; CREN=1; SREN=0;
		bsf	TXSTA,BRGH                      ;       TX9=0; RX9=0; TXEN=1;
	ELSE
		bcf TXSTA,BRGH
	ENDIF
	bsf	TXSTA,TXEN
	movlw	DIVIDER					; baud rate generator
	movwf	SPBRG
	IFDEF PIN
		bsf	TRIGGER				; for PIN:  setting of TRIS for selected pin
		bcf	STATUS,RP0
		btfss	TRIGGER
		goto    receive				; go to bootloader
		goto	user_restore		        ; run user program
	ELSE
		bcf	STATUS,RP0
		movlw	TIMEOUT+1			; for TIME: set timeout
		movwf	time
		movlw	T1SU
		movwf	T1CON				; TIMER1 on, internal clock, prescale T1PS
		bsf     PIR1,TMR1IF
		call	getbyte				; wait for IDENT
		xorlw	IDENT
		btfss	STATUS,Z
		goto	user_restore
		clrf	time				; no more wait for IDENT
		goto	inst_ident			; bootloader identified, send of IDACK
	ENDIF

;------------------------------------------------------------------------------
receive							; programming
	call	getbyte					; get byte from USART
	movwf	temp
	xorlw	WRITE
	btfsc	STATUS,Z
	goto	inst_write				; write instruction
	movf	temp,w
	xorlw	IDENT
	btfsc	STATUS,Z
	goto	inst_ident				; identification instruction
	movf	temp,w
	xorlw	DONE
	btfss	STATUS,Z				; done instruction ?
	goto	receive

;------------------------------------------------------------------------------
inst_done						; very end of programming
;------------------------------------------------------------------------------
	movlw	WR_OK
  	call	putbyte					; send of byte
	movlw	TIMEOUT+1
	movwf	time
  	call	getbyte                                 ; has built in timeout - waits until done
;------------------------------------------------------------------------------
user_restore
	clrf    T1CON					; shuts off TIMER1
	clrf    RCSTA
	bsf     STATUS,RP0
	clrf    TXSTA					; restores USART to reset condition
	bcf     STATUS,RP0
	clrf    PIR1
	goto	UserStart				; run user program

;------------------------------------------------------------------------------
inst_ident
	movlw	IDACK					; send IDACK
	goto	send_byte
;------------------------------------------------------------------------------
inst_write
	call	getbyte
	movwf	address+1				; high byte of address
	call	getbyte
	movwf	address					; low byte of address
	call	getbyte
	movwf	amount					; number of bytes -> amount -> count
	movwf 	count
	call	getbyte					; checksum -> chk2
	movwf	chk2
	clrf	chk1					; chk1 = 0

;>>> ADAPT1.2
	movlw	0x21					; if (0x2100 <= tmpaddr <= 0x21FF) ..........
	banksel	address
	subwf	address+1,w
	btfsc	STATUS,Z
	goto	adapt12_1end				; no action if data eeprom

	movf	address,w
	andlw	0x03					; Bit 0,1 der Startadresse
	movwf	help
	movwf	lcount					; Zähler für späteres Lesen
	bcf	STATUS,C
	rlf	help					; buffer-offset: (adress & 0x03) * 2
	movlw	buff
	addwf	help,w
	goto	adapt12_2end
adapt12_1end						; no action if data eeprom
	movlw 	buff
adapt12_2end						; no action if data eeprom
	movwf 	FSR
;>>> ADAPT1.2 end
							; FSR pointer = buff
receive_data
	call	getbyte					; receive next byte -> buff[FSR]
	movwf	INDF
	addwf	chk1,f					; chk1 := chk1 + buff[FSR]
	incf  	FSR,f					; FSR++
	decfsz 	count,f
	goto receive_data				; repeat until (--count==0)
checksum
	movf	chk1,w
	xorwf	chk2,w					; if (chk1 != chk2)
	movlw 	DATA_BAD
	btfss	STATUS,Z
	goto	send_byte				; checksum WRONG
checksum_ok
	movlw	DATA_OK					; checksum OK
	call	putbyte
write_byte
	call	write_eeprom				; write to eeprom
	iorlw	0
	movlw 	WR_OK					; writing OK
	btfsc	STATUS,Z
	movlw	WR_BAD					; writing WRONG

;------------------------------------------------------------------------------
send_byte
	call	putbyte					; send of byte
	goto	receive					; go to receive from UART
;------------------------------------------------------------------------------

;************************* putbyte subroutine *********************************
putbyte
	clrwdt
	btfss	PIR1,TXIF				; while(!TXIF)
	goto	putbyte
	movwf	TXREG					; TXREG = byte
	return

;************************* getbyte subroutine *********************************
getbyte
	clrwdt
	IFNDEF PIN					; for TIME
		movf	time,w
		btfsc	STATUS,Z			; check for time==0
		goto	getbyte3
		btfss	PIR1,TMR1IF			; check for TIMER1 overflow
		goto	getbyte3			; no overflow
		bcf	T1CON,TMR1ON			; timeout 0.1 sec
		decfsz	time,f				; time--
		goto	getbyte2
		retlw 	0				; if time==0 then return
getbyte2
		bcf	PIR1,TMR1IF
		movlw	high TIMER
		movwf	TMR1H				; preset TIMER1 for 0.1s timeout
		bsf	T1CON,TMR1ON
	ENDIF
getbyte3
	btfss	PIR1,RCIF				; while(!RCIF)
	goto	getbyte
	movf	RCREG,w					; RCREG
	return

;******************** write eeprom subroutine *********************************
write_eeprom

;>>> ADAPT1.2
	movlw	0x21					; if (0x2100 <= tmpaddr <= 0x21FF) ..........
	subwf	address+1,w
	btfsc	STATUS,Z
	goto	adapt12_3end				; no action if data eeprom

	banksel	EECON1
	bsf	EECON1,EEPGD				; EEPGD = 1 -> program memory
	banksel	address
	movf	address,w
	andlw	0xfc
	bsf	STATUS,RP1
	movwf	EEADR					; EEADR = low addr
	bcf	STATUS,RP1
	movf	address+1,w
	bsf	STATUS,RP1
	movwf	EEADRH					; EEADRH = high addr
	movlw 	buff
	movwf 	FSR

;read memory and write to buffer front
adapt12_loop1
	bcf	STATUS,RP1
	movf	lcount,w
	btfsc	STATUS,Z
	goto	adapt12_4end				; no more read operation from Flash Memory
	banksel	EECON1
	bsf	EECON1,RD
	nop
	nop
	bcf	STATUS,RP0
	movf   	EEDATH,w
	movwf	INDF
	incf  	FSR,f					; FSR++
	movf   	EEDATA,w
	movwf	INDF
	incf  	FSR,f					; FSR++
	incf	EEADR
	bcf	STATUS,RP1
	decf	lcount
	decf	address
	incf	amount
	incf	amount
	goto	adapt12_loop1

;read memory and write to buffer tail
adapt12_4end
	movf	amount,w
	movwf	help
	addlw	buff					; Schreibadresse in Puffer
	movwf	FSR
	bcf	STATUS,C
	rrf	help					; Anzahl der Programmwoerter

	movf	address+1,w
	bsf	STATUS,RP1
	movwf	EEADRH
	bcf	STATUS,RP1
	movf	address,w
	bsf	STATUS,RP1
	movwf	EEADR					; EEADR/EEADRH steht auf erster zu schreibender Adresse
	bcf	STATUS,RP1
	movf	help,w
	bsf	STATUS,RP1
	addwf	EEADR,f
	btfsc	STATUS,Z
 	incf	EEADRH,f

adapt12_loop2
	bsf	STATUS,RP1
	movf	EEADR,w
	andlw	0x03					; while (EEADR & 0x03) != 0
	btfsc	STATUS,Z
	goto	adapt12_3end

	bsf	STATUS,RP0
	bsf	EECON1,RD
	nop
	nop
	bcf	STATUS,RP0
	movf   	EEDATH,w
	movwf	INDF
	incf  	FSR,f					; FSR++
	movf   	EEDATA,w
	movwf	INDF
	incf  	FSR,f					; FSR++
	incf	EEADR
	bcf	STATUS,RP1
	incf	amount
	incf	amount
	goto	adapt12_loop2
adapt12_3end
;>>> ADAPT1.2 end

	movf	address,w
	movwf	tmpaddr					; tmpaddr = address
	movf	address+1,w
	movwf	tmpaddr+1
	clrf	count					; count=0
write_loop
	movlw	NumRetries+1				; retry = NumRetries+1
	movwf	retry
w_e_l_1
	movf	amount,w
	subwf	count,w					; while (count<amount)
	btfsc	STATUS,C
	retlw	1
	movf	count,w
	addlw	buff					; set buffer pointer
	movwf	FSR
w_e_l_2
	movlw	0x21					; if (0x2100 <= tmpaddr <= 0x21FF) ..........
	subwf	tmpaddr+1,w
	bsf	STATUS,RP1
	bsf	STATUS,RP0				; (bank3)
	btfsc	STATUS,Z
	 goto	data_eeprom				; goto data_eeprom
program_eeprom
	bsf		EECON1,EEPGD			; EEPGD = 1 -> program memory
	clrf	STATUS
	movlw	high (LoaderStart)			; if (tmpaddr >= LoaderStart) ...............
	subwf	tmpaddr+1,w
	movlw	low (LoaderStart)			; mask Booloader, [ICD-Debugger],
	btfsc	STATUS,Z				;      __IDLOCS & __CONFIG
	subwf	tmpaddr,w
	btfsc	STATUS,C
	goto 	next_adr				; next address
	goto  	w_e_l_3
data_eeprom
	bcf	EECON1,EEPGD				; EEPGD = 0 -> data memory
	clrf	STATUS
w_e_l_3
	movf	tmpaddr,w
	bsf	STATUS,RP1
	movwf	EEADR					; EEADR = low tmpaddr
	bcf	STATUS,RP1
	movf	tmpaddr+1,w				;	if (tmpaddr < 0x0004) .....................
	btfss 	STATUS,Z
	goto 	w_e_l_4
;;;;;;;;;vvv 2005-12-22mn
	IFDEF PICC_LITE_950
	movlw	8
        ELSE
	movlw	4
        ENDIF
;;;;;;;;;^^^ 2005-12-22mn
	subwf	tmpaddr,w
	btfsc	STATUS,C
	goto	w_e_l_4
	bsf 	STATUS,RP1                  		; (bank3)
	bsf 	STATUS,RP0
	btfss 	EECON1,EEPGD              		; skip if (EEPGD)
	goto	w_e_l_31
	bcf	STATUS,RP0                  		; (bank2)
;>>> ADAPT
	IFDEF __16F873A
		movlw low UserStart+2						; EEADRL + low UserStart+2
  	ELSE
		IFDEF __16F876A
			movlw low UserStart+1					; EEADRL + low UserStart+1
    		ELSE
			IFDEF __16F877A
				movlw low UserStart+1				; EEADRL + low UserStart+1
    			ELSE
        			ERROR   "incorrect type of microcontroller"
    			ENDIF
    		ENDIF
    	ENDIF
;>>>
	addwf 	EEADR,f                   		; (relocated first 4 user instructions)
w_e_l_31
	clrf	STATUS                    		; (bank0)
	movlw	high UserStart				; EEADRH = high UserStart
	goto	w_e_l_5
w_e_l_4
	movf	tmpaddr+1,w				; EEADRH = high tmpaddr
w_e_l_5
	bsf	STATUS,RP1
	movwf	EEADRH					; set EEADRH
	movf	INDF,w
	movwf	EEDATH					; EEDATH = buff[count]
	incf  	FSR,f
	movf  	INDF,w
	movwf	EEDATA					; EEDATA = buff[count+1]
	bsf	STATUS,RP0
	bsf	EECON1,WREN				; WREN=1
	movlw	0x55					; EECON2 = 0x55
	movwf	EECON2
	movlw	0xAA					; EECON2 = 0xAA
	movwf	EECON2
	bsf	EECON1,WR				;	WR=1
	nop						; instructions are ignored
	nop						; microcontroller waits for a complete write
	clrf	STATUS

;>>> ADAPT
	banksel	EECON1
	btfsc   EECON1,EEPGD               		; skip if (EEPGD=0 / write to EEPROM)
	goto	ver_flash
;>>>

	banksel	PIR2
wait_write
	clrwdt
	btfss	PIR2,EEIF				; necessary for a write to data eeprom
	goto	wait_write
	bcf	PIR2,EEIF
	bsf 	STATUS,RP0				; (bank3)
	bsf 	STATUS,RP1
	bcf	EECON1,WREN				; WREN=0


	bsf	EECON1,RD				; RD=1
	nop
	nop
	bcf 	STATUS,RP0				; (bank2)
	decf  	FSR,f
	movf  	INDF,w					; if ((EEDATH != buff[count]) || (EEDATA != buff[count+1]))
	xorwf 	EEDATH,w
	btfss 	STATUS,Z
	goto	w_e_l_6					; repeat write
	incf  	FSR,f
	movf  	INDF,w
	xorwf 	EEDATA,w
	btfsc	STATUS,Z
	goto	next_adr				; verification OK, next address
w_e_l_6
	clrf	STATUS					; (bank0)
	decfsz	retry,f
	goto	w_e_l_1					; if (--retry != 0) repeat write
	retlw	0					; else return 0 (BAD)
next_adr
;>>> ADAPT
    	banksel count
;	bcf 	STATUS,RP1
;>>>
	movlw	2					;	count := count + 2
	addwf	count,f
	incf	tmpaddr,f				; tmpaddr := tmpaddr + 1
	btfsc	STATUS,Z
	incf	tmpaddr+1,f
	goto	write_loop

;>>> ADAPT
ver_flash
	banksel	PIR2
	bcf	PIR2,EEIF
	banksel	EECON1
	bcf	EECON1,WREN				; WREN=0

    	banksel EEADR
    	movf    EEADR,w
    	andlw   0x03
    	sublw   0x03
    	btfss   STATUS,Z
    	goto    next_adr        			; if (EEADR & 0x03) = 0x03

    	movlw   3
    	subwf   EEADR,f         			; EEADR <- EEADR - 3

    	banksel help
    	movlw   4
    	movwf   help            			; help <- 4

    	movlw   7
    	subwf   FSR,f           			; FSR <- FSR - 7

ver_fl_1
    	nop
	banksel	EECON1
	bsf	EECON1,RD				; RD=1
	nop
	nop
	nop
	nop
	nop
	nop

	movf    INDF,w					; if ((EEDATH != buff[count]) || (EEDATA != buff[count+1]))
	banksel	EEDATH
	xorwf   EEDATH,w
	btfss   STATUS,Z
	goto	ver_fl_2				; repeat write
	incf    FSR,f
	movf    INDF,w
	xorwf   EEDATA,w
	btfsc	STATUS,Z
	goto	ver_fl_3
ver_fl_2                    				; invalid verify
    	banksel tmpaddr
    	movlw   0x0fc
    	andwf   tmpaddr,f       			; clear least significant two bits in [tmpaddr]

    	movlw   6
    	subwf   count,f           			; count <- count - 6

    	goto    w_e_l_6

ver_fl_3
    	banksel EEADR
	incf    EEADR,f
	incf    FSR,f

    	banksel help
    	decfsz  help,f
    	goto    ver_fl_1
   	goto    next_adr

;>>>



;------------------------------------------------------------------------------
	END

