f_addVectors:
    sub.l   #4, sp          ; def result as Vec2

    move.w  12(sp), (sp)    ; = first
    move.w  14(sp), 2(sp)

    add.w   10(sp), 2(sp)   ; result.x = result.x + second.x
    add.w   8(sp), (sp)     ; result.y = result.y + second.y

    move.w  (sp), 16(sp)    ; return result
    move.w  2(sp), 18(sp)
    add.l   #4, sp          ; Callee cleans up its own locals
    rts

_main:
    sub.l   #4, sp          ; def test1 as Vec2

    move.w  #2, 2(sp)       ; = Vec2( 2, 2 )
    move.w  #2, (sp)

    sub.l   #4, sp          ; def test2 as Vec2

    move.w  #2, 2(sp)       ; = Vec2( 2, 2 )
    move.w  #2, (sp)

    sub.l   #4, sp          ; def result as Vec2

    sub.l   #4, sp          ; addVectors( test1, test2 )
    move.w  #0, 2(sp)
    move.w  #0, (sp)
    sub.l   #4, sp
    move.w  18(sp), 2(sp)
    move.w  16(sp), (sp)
    sub.l   #4, sp
    move.w  18(sp), 2(sp)
    move.w  16(sp), (sp)
    jsr     f_addVectors
    add.l   #8, sp          ; Caller cleans up arguments

    move.w  2(sp), 6(sp)    ; = addVectors( test1, test2 )
    move.w  (sp), 4(sp)
    add.l   #4, sp          ; Clean up temporary function call result
