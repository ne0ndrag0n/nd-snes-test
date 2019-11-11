; [ashley@BPC-V snes-test]$ ca65 main.s
; [ashley@BPC-V snes-test]$ ld65 -C lorom128.cfg -o ca65.smc main.o
; [ashley@BPC-V snes-test]$

.p816
.i16
.a8

.segment "HEADER"
    .byte "Neon Dragon Research"

.segment "ROMINFO"
    .byte $30           ; LoROM
    .byte 0             ; No SRAM
    .byte $07           ; 128K EOM
    .byte 0,0,0,0
    .word $AAAA,$5555   ; Garbage checksum/complement

.segment "VECTORS"
    ;  FFE0  Zerofilled (or ID "XBOO" for WRAM-Boot compatible files)
    .word 0, 0
    ;  FFE4  COP vector     (65C816 mode) (COP opcode)
    .word $FFFF
    ;  FFE6  BRK vector     (65C816 mode) (BRK opcode)
    .word $FFFF
    ;  FFE8  ABORT vector   (65C816 mode) (not used in SNES)
    .word $FFFF
    ;  FFEA  NMI vector     (65C816 mode) (SNES V-Blank Interrupt)
    .word vblank
    ;  FFEC  RESET vector   (65C816 mode) (not used in SNES)
    .word $FFFF
    ;  FFEE  IRQ vector     (65C816 mode) (SNES H/V-Timer or External Interrupt)
    .word hblank
    ;  FFF0  Zerofilled
    .word 0, 0
    ;  FFF4  COP vector     (6502 mode)
    .word $FFFF
    ;  FFF6  Zerofilled
    .word 0
    ;  FFF8  ABORT vector   (6502 mode) (not used in SNES)
    .word $FFFF
    ;  FFFA  NMI vector     (6502 mode)
    .word $FFFF
    ;  FFFC  RESET vector   (6502 mode) (CPU is always in 6502 mode on RESET)
    .word reset
    ;  FFFE  IRQ/BRK vector (6502 mode)
    .word $FFFF

.segment "CODE"

reset:
    clc         ; native mode
    xce
    rep #$10    ; Index regs 16-bit
    sep #$20    ; Acc 8-bit

    ldx #$33    ; Clear PPU Registers
@loop:
    stz $2100,x
    stz $4200,x
    dex
    bpl @loop

    lda #$E0    ; Write Colour #0 to $03E0
    sta $2122
    lda #$03
    sta $2122

    lda #$0F    ; Max brightness
    sta $2100

main:
    jmp main

vblank:
    ; nothing happens here yet.
    rti

hblank:
    ; nothing happens here yet.
    rti

initSnes:
    sei         ; Disable interrupts
    clc         ; Disable emulation mode
    xce

    rep #$38    ; A, X are set to 16-bit
                ; Decimal mode off

    ldx #$1FFF  ; Setup stack pointer
    txs

    phk         ; Set data bank register to program bank register
    plb

    lda #$0000  ; Direct (zero) page is 0
    tcd

    sep #$20    ; A, X are set to 8-bit
    rep #$10

    lda #$8F    ; Disable display and set brightness to 15
    sta $2100

    ldx #$0001    ; Clear out registers $2101-$210C
:
    stz $2100,x
    inx
    cpx #$000D
    bne :-
:
    stz $2100,x   ; Registers $210D-2114 are double-writes
    stz $2100,x
    inx
    cpx #$0015
    bne :-

    lda #$80
    sta $2115

    stz $2116   ; vram address set to 0
    stz $2117

    ; $2118 and $2119 are the vram data registers

    stz $211A   ; reset mode7 register $211A

    ldx #$001B  ; Clear out registers $211B-$2120
:
    stz $2100,x ; TODO: Only $211F and $2120 are doublewrite??
    stz $2100,x
    inx
    cpx #$0021
    bne :-

    ; $2121 doesn't need init according to InitSNES.asm
    ; $2122 is a colour data register

    ldx #$0023      ; clear out registers $2123-2133
:
    stz $2100,x
    inx
    cpx #$0034
    bne :-

    ; skip a bunch of readonly registers...

    stz $213E       ; Use register $213E elect ppu master/slave mode

    ; skip a bunch of readonly and data registers...

    stz $4200       ; Disable vblank, v/h counter, and joypad read

    lda #$FF
    sta $4201       ; Set up the controller using programmable i/o port

    ; 4207-4208 contain hblank timer setting
    ; 4209-420A contain vblank timer setting
    ; These are left alone, but need to be set if either is enabled

    stz $420B       ; Turn off all DMA channels
    stz $420C       ; Turn off all HDMA channels
    stz $420D       ; 120ns or slower EPROM

    ; Omitting an lda from $4210 here

    ; todo
    rts