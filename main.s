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
    .word 0, 0, 0, 0, 0, 0, 0, 0
    .word 0, 0, 0, 0, 0, 0, reset, 0

.segment "CODE"

reset:
    wdm $00

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

initSnes:
    sei         ; Disable interrupts
    ; todo
    rts