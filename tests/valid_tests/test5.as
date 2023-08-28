; More complex tests with all operations
.entry COMPLEXSTART
.extern COMPLEXFUNC
COMPLEXMAIN: mov @r1, @r2
LOOPA:       cmp @r3, @r4
              bne ENDLOOPA
              add @r5, @r6
              sub @r7, @r0
              jsr LOOPB
              prn STR10
              lea DATAA, @r5
LOOPB:       clr @r1
              not @r2
COMPLEXSTART:  rts
ENDLOOPA:    stop
STR10:       .string "complextest"
DATAA:       .data -10, -5, 0, 5, 10
DATAB:       .data 100, 200, 300
