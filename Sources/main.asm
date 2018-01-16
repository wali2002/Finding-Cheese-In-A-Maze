; export symbols
            XDEF asm_main , StartAddr , SoftXIRQ , intrNum
            XDEF initial
            ;XDEF sensor
            XDEF forward
            ;XDEF cnt
            ;XDEF cnt2
            XDEF turn_left
            XDEF turn_right
            XDEF straighten
            XDEF mtr_go
            ;XDEF forward2
            XDEF mtr_g
            ;XDEF mtr_spin
            XDEF sns_l
            XDEF sns_r
            XDEF sns_f
            XDEF wsns_l
            XDEF wsns_r
            XDEF rst_sns
            XDEF right_adjust
            XDEF left_adjust
            XDEF reverse
            XDEF stop_dead
            ;XDEF asm_main
            XDEF wait
            ;XDEF printPC
            XDEF reset_cnt
            

; Include derivative-specific definitions 
		INCLUDE 'derivative.inc'  		

SoftXIRQ equ $3E74
; equates section
ADdelay:   equ $600 ;Delay value for A/D setup

SCFflag:   equ $80
admask2:   equ $80  ;ADPU = 1 Enable A/D
admask3:   equ $28  ;do 5 conversions - 5 sensors
admask4:   equ $E5  ;8 bit conver,16 A/D conver cycles, prescale 00101
admask5:   equ $B0  ;DJM=1(right),unsigned,continue convt,multi @ 0
Delay:     equ $600 ;Delay value
printf     equ $EE88 ; printf function 
out2hex    equ $EE9C ; output 2 chars to screen; in B
out4hex    equ $EEA0 ; output 4 chars to screen; in D

lcd_banner equ $0FEE ; LCD banner 
lcd_clear  equ $0FE4 ; clear the display
lcd_cmd    equ $0FEA ; send a command
lcd_init   equ $0FEC ; initialize the LCD
lcd_putc   equ $0FE8 ; send a single char 
lcd_puts:  equ $0FE6 ; display my message		    

MY_EXTENDED_RAM: SECTION
; Insert here your data definition. For demonstration, temp_byte is used.
intrNum:    DS.W   1       ; interrupt number
temp_byte:  DS.B   1
MsgSens:    DC.B "Sensor Data" , $00
MsgDelay:   DC.B "Delay Msg"   , $00
LcdInit:    DC.B "Inital"      , $00
MsgFrwd:    DC.B "Forward"     , $00
MsgRevs:    DC.B "Reverse"     , $00
MsgLeft:    DC.B "Left"        , $00
MsgRite:    DC.B "Rite"        , $00
MsgStop:    DC.B "Stop"        , $00
MsgDisp:    DC.B "Display"     , $00
Sns         DC.B $00
StartAddr:  DC.W   asm_main


; code section
MyCode:     SECTION
; this assembly routine is called by the C/C++ application
asm_main:
            LDAB  PORTB              ; read port T
            ANDB  #%11110000       ; clear high nibble
            LSRB                   ; shift to low nibble
            LSRB
            LSRB
            LSRB
            CLRA
            STD   intrNum         ; save the intr num
            BNE   newIntr         ; jump if intr
            JMP   $F000           ; THIS IS A RESET!
newIntr:    
            
            RTI                   ; Back to the C prog!
            
            
            
initial:
         ; Port T, bits 0,1,2,3 - used for motor direction (O/P)
         ; Port T, bits 4,5,6,7 - used for IRQ input determination (I/P)

      LDX   #0
      JSR   [lcd_init,x]   ; Initialize the LCD
      
      
      
      ;LDAA  #$00
      ;STAA  DDRH
      LDAA  #$F0
      STAA DDRT
      ;bclr  DDRT,$F0   ; bits0-3=input, bits4-7=output
      LDAA  #$00
      STAA  RDRT       ; Reduced drive disabled
     ; LDAA  #$F0
     ; STAA  PERT       ; pull-up selected for I/P
     ; LDAA  #$00
      ;STAA  PPST       ; enable pull-up
      ;LDAA  #%00010000 ; 01010000 - to combine PACN3 and PACN2 for 16-bit counter
      ;STAA  PACTL
      ;LDAA  #%00000000
      ;STAA  PBCTL
      movb  #$0F,ICPAR
      LDAA  #$55
      STAA  TCTL4
      
      
; Set up the PWM ports - port P, bits 4-7
; The crystal is 24MHz - -  Fbus freq = 12Mhz
      LDAA  #$22         
      STAA  PWMPRCLK   ; ClockA&B=FBus/4=3000K 
      LDAA  #$F0       ; 1111 0000 
      STAA  PWMCLK     ; ClockSB - ch.7,6 ; ClockSA - ch.5,4 
      LDAA  #25
      STAA  PWMSCLA
      STAA  PWMSCLB    ; ClockSA&B=ClockA&B/2*15=100KHz
      LDAA  #$F0       ; 1111 0000
      STAA  PWMPOL     ; high, then low for polarity
      LDAA  #$00
      STAA  PWMCAE     ; left aligned
      LDAA  #$00
      STAA  PWMCTL     ; 8-bit chan, PWM during freeze and wait
      LDAA  #200
      STAA  PWMPER4    ; PWM_Freq=ClockSA&B/200 = 0.5KHz  
      STAA  PWMPER5
      STAA  PWMPER6    
      STAA  PWMPER7
      
           
      movb #$00,ATD0DIEN ;disable digital inputs
      movb #admask2,ATD0CTL2 ;configure ATD0CT12
      ldx #Delay ;Delay 100 usec - A/D powers up
loop: dex
      bne loop
      movb #admask3,ATD0CTL3 ;configure ATD0CT13
      movb #admask4,ATD0CTL4 ;configure ATD0CT14
       LDD   #LcdInit
      LDX   #0
      JSR   [lcd_puts,x]
      rts
                  
forward:      
      LDAA  #$F0       ; FWD=F0  BAK=00   
      ;(bit/mtr : 0/1Fleft 1/2Fright 2/3RR 3/4RL)
      STAA  PTT        ; set direction forward,forward
; start the motors (second half of 'initial')
mtr_go:
      LDAA  #60        ; duty cycle = PWMDTY/PWMPER (x/200)
      STAA  PWMDTY4    ; min value = 50
      STAA  PWMDTY6    ; motor 2
      LDAA  #60       ; right wheel        ;
      STAA  PWMDTY5 
      STAA  PWMDTY7    ; motor 1        
      LDAA  #$00
      STAA  PWMCNT4    ; clear the channel
      STAA  PWMCNT5    ; clear the channel
      STAA  PWMCNT6    ; clear the channel
      STAA  PWMCNT7    ; clear the channel       
      BSET  PWME,%11110000 ; turn on ch. 4,5,6,7 - start PWMCNT
      RTS
      
      
mtr_g:
      LDAA  #50        ; duty cycle = PWMDTY/PWMPER (x/200)
      STAA  PWMDTY4    ; min value = 50
      STAA  PWMDTY6    ; motor 2
      LDAA  #50       ; right wheel        ;
      STAA  PWMDTY5 
      STAA  PWMDTY7    ; motor 1        
      LDAA  #$00
      STAA  PWMCNT4    ; clear the channel
      STAA  PWMCNT5    ; clear the channel
      STAA  PWMCNT6    ; clear the channel
      STAA  PWMCNT7    ; clear the channel       
      BSET  PWME,%11110000 ; turn on ch. 4,5,6,7 - start PWMCNT
      RTS      

     

stop_dead:
; disable the PWM to 0%
      LDAA  #$00       ; FWD=0F  BAK=00   
      STAA  PTT
stop_:            
      LDAA  #00        ; duty cycle = PWMDTY/PWMPER (x/200)
      STAA  PWMDTY4    
      STAA  PWMDTY6    ; motor 2
      LDAA  #00        
      STAA  PWMDTY5 
      STAA  PWMDTY7    ; motor 1        
      LDAA  #$00
      STAA  PWMCNT4    ; clear the channel
      STAA  PWMCNT5    ; clear the channel
      STAA  PWMCNT6    ; clear the channel
      STAA  PWMCNT7    ; clear the channel       
      BSET  PWME,%11110000 ; turn on ch. 4,5,6,7 - start PWMCNT
      
      RTS
      
wait:     TBA        ; value comes in D
          PSHX
          LSLA         ; shift left twice (x4) 
          LSLA
          ABA          ; A=A+B   (4x + 1x = 5X)
Delay1:   LDX   #$FFFF  ;delay=19.1ms@24MHz
Dly_20:   NOP
          NOP
          NOP
          DEX
          BNE   Dly_20
          DECA
          BNE   Delay1  ; repeat inner
;          LDD  #MsgDelay   ;load LCD title
;          LDX  #0
;          JSR  [lcd_puts,x]
          PULX
          RTS
          

left_adjust:
      LDAA  #55        ; duty cycle = PWMDTY/PWMPER (x/200)
      STAA  PWMDTY4    ; min value = 50
      STAA  PWMDTY6    ; motor 2
      LDAA  #00 ; right wheel        ;
      STAA  PWMDTY5 
      STAA  PWMDTY7    ; motor 1        
      LDAA  #$00
      STAA  PWMCNT4    ; clear the channel
      STAA  PWMCNT5    ; clear the channel
      STAA  PWMCNT6    ; clear the channel
      STAA  PWMCNT7    ; clear the channel       
      BSET  PWME,%11110000 ; turn on ch. 4,5,6,7 - start PWMCNT
      RTS
      
right_adjust:
      LDAA  #00        ; duty cycle = PWMDTY/PWMPER (x/200)
      STAA  PWMDTY4    ; min value = 50
      STAA  PWMDTY6    ; motor 2
      LDAA  #55 ; right wheel        ;
      STAA  PWMDTY5 
      STAA  PWMDTY7    ; motor 1        
      LDAA  #$00
      STAA  PWMCNT4    ; clear the channel
      STAA  PWMCNT5    ; clear the channel
      STAA  PWMCNT6    ; clear the channel
      STAA  PWMCNT7    ; clear the channel       
      BSET  PWME,%11110000 ; turn on ch. 4,5,6,7 - start PWMCNT
      RTS      



;******************************************************************           
sns_l:
      LDAb  PTH
      ANDb  #%00000010
      ldaa  #$00
      RTS
      
sns_r:
      LDAb  PTH
      ANDb  #%00000001
      ldaa  #$00
      RTS
      
sns_f:
      LDAb  PTH
      ANDb  #%00000100
      ldaa  #$00
      RTS         
      
rst_sns:
      LDAA  #00
      STAA  Sns
      RTS      
;******************************************************************

wsns_l:
       LDAb PORTB
       ANDb #%00000001
       LDAa #$00
       RTS

wsns_r:
       LDAB PTH
       ANDB #%00001000
       LDAA #$00
       RTS
;*******************************************************************
      
reset_cnt:  LDAA  #00
            STAA  PACN0
            STAA  PACN3
            RTS
         
      
turn_left:
      LDAA  #$30
      STAA  PTT        
      RTS
turn_right:
      LDAA  #$C0
      STAA  PTT        
      RTS
straighten:
      LDAA  #$F0           
      STAA  PTT        
      RTS            
            
            
reverse:
    LDAA #$00
    STAA PTT
    RTS 