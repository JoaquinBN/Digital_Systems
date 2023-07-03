LIST P = PIC18F4321 F = INHX32
    #include <p18f4321.inc>
    CONFIG OSC = INTIO2
    CONFIG PBADEN = DIG
    CONFIG WDT = OFF
    CONFIG DEBUG = ON
    CONFIG LVP = OFF

    ; DEFINE VARIABLES

    mode EQU 0x01 ; 0 = Manual Mode, 1 = Record mode, 2 = Automatic mode
    debounce_counter EQU 0x02
    read_X EQU 0x03
    read_Y EQU 0x04
    speed EQU 0x05 ; 0-1: 00 = LOW, 01 = MEDIUM, 10 = HIGH, 11 = 0, 2 = rear/!front
    dir EQU 0x06; 0 = center, 1 = 0:left;1:right, 2-3-4-5 = how much turn
    delay_L EQU 0x07
    delay_H EQU 0x08
    record_L EQU 0x09
    record_H EQU 0x0A
    steps EQU 0x0B
    pwmX_H EQU 0x0C
    pwmX_L EQU 0x0D
    aux1 EQU 0x0E
    aux2 EQU 0x0F
    aux3 EQU 0x21
    aux4 EQU 0x22
    pwmY_L EQU 0x23
    pwmY_H EQU 0x24
    prev_X EQU 0x25
    prev_Y EQU 0x26
    alarm_L EQU 0x27
    alarm_H EQU 0x28
    alarm_clk EQU 0x29
    aux5 EQU 0x2A
    aux6 EQU 0x2B
    TABLE7s EQU 0x10
    ORG TABLE7s
       ; L, M - 0x10, 0x11
       DB b'0001101', b'1110101'
       ; H, 0 - 0x12, 0x13
       DB b'0110111', b'1111101'
    
    ; VERIFY REDIRECTS
    ORG 0x0000
    GOTO MAIN
    ORG 0x0008
    GOTO HIGH_RSI
    ORG 0x0018
    RETFIE FAST


HIGH_RSI
    BTFSC INTCON, TMR0IF,0
    GOTO TMR0_INTERRUPT
    
    RETFIE FAST

TMR0_INTERRUPT
    BCF INTCON,TMR0IF,0
    INCF delay_L,1,0
    MOVLW .255
    SUBWF delay_L,0,0
    BTFSC STATUS,Z,0
    INCF delay_H,1,0
    
    ;alarm timing
    BTFSS mode,5,0
    INCF alarm_L,1,0
    MOVLW .255
    SUBWF alarm_L,0,0
    BTFSC STATUS,Z,0
    INCF alarm_H,1,0
    
    INCF debounce_counter,1,0
    ;reset the value of TMR0 to decimal value of 1536 in order to ensure an interrupt each 8ms
    
    MOVLW LOW(.25536)
    MOVWF TMR0L,0
    MOVLW HIGH(.25536)
    MOVWF TMR0H,0
   
    MOVFF pwmX_H, aux1
    MOVFF pwmX_L, aux2
    BSF LATE,0,0
    CALL SERVO_X
    BCF LATE,0,0
    
    MOVFF pwmY_H, aux3
    MOVFF pwmY_L, aux4
    BSF LATE,1,0
    CALL SERVO_Y
    BCF LATE,1,0
    
    MOVLW .11
    SUBWF alarm_H,0,0
    BTFSS STATUS,Z,0
    GOTO ALARM_OFF
    
    MOVLW .184
    SUBWF alarm_L,0,0
    BTFSS STATUS,Z,0
    GOTO ALARM_OFF
    
    BSF mode,5,0
    INCF alarm_clk,1,0
    
    MOVLW .25
    SUBWF alarm_clk,0,0
    BTFSS STATUS,Z,0
    RETFIE FAST
    
    BTG LATA,3,0
    CLRF alarm_clk,0
    RETFIE FAST
    
    ALARM_OFF
    BCF mode,5,0
    CLRF alarm_clk,0
    BCF LATA,3,0
    RETFIE FAST
    
    
    
SERVO_X
    CALL COUNT_X
    ;reset pwm_L after reaching 0
    MOVLW .255
    MOVWF aux2,0
    
    ;check if pwm_H reached
    MOVLW .0
    SUBWF aux1,0,0
    BTFSC STATUS,Z,0
    RETURN
    
    ;increment and repeat if not reached
    DECF aux1,1,0
    GOTO SERVO_X
    
COUNT_X
   MOVLW .0
   SUBWF aux2,0,0
   BTFSC STATUS,Z,0
   RETURN
   DECF aux2,1,0
   GOTO COUNT_X
    
SERVO_Y
    CALL COUNT_Y
    ;reset pwm_L after reaching 0
    MOVLW .255
    MOVWF aux4,0
    
    ;check if pwm_H reached
    MOVLW .0
    SUBWF aux3,0,0
    BTFSC STATUS,Z,0
    RETURN
    
    ;increment and repeat if not reached
    DECF aux3,1,0
    GOTO SERVO_Y
    
COUNT_Y
   MOVLW .0
   SUBWF aux4,0,0
   BTFSC STATUS,Z,0
   RETURN
   DECF aux4,1,0
   GOTO COUNT_Y
    
INIT_OSC
    MOVLW b'01110010'   
    MOVWF  OSCCON,0 
    MOVLW b'10011000' ; 16 bit timer - no prescaler 
    MOVWF T0CON,0
    RETURN

INIT_INTERRUPTS
    BCF RCON,IPEN,0 ; no priorities so everything is high priority
    BCF INTCON2, RBPU,0
    BSF INTCON,GIE,0
    BSF INTCON,PEIE,0
    BSF INTCON2,INTEDG0,0
    BSF INTCON2,INTEDG1,0
    BSF INTCON2, TMR0IP,0
    BSF INTCON, TMR0IE,0
    
    RETURN

INIT_PORTS
    MOVLW b'00000011'
    MOVWF ADCON0,0
    MOVLW b'00001110'
    MOVWF ADCON1,0
    MOVLW b'00001100'
    MOVWF ADCON2,0

    ;SET JOYSTICK AS INPUT
    BSF TRISA,0,0
    BSF TRISA,1,0
    ;SET PCB LED OUTPUT
    BCF TRISA,3,0
    BCF TRISA,4,0
    ;SET LED BAR OUTPUT
    BCF TRISC,0,0
    BCF TRISC,1,0
    BCF TRISC,2,0
    BCF TRISC,3,0
    BCF TRISD,7,0
    BCF TRISB,3,0
    BCF TRISB,4,0
    BCF TRISB,5,0
    BCF TRISB,6,0
    BCF TRISB,7,0
    ;SET RGB LED OUTPUT
    BCF TRISC,4,0
    BCF TRISC,5,0
    BCF TRISC,6,0
    ;SET MANUAL AND RECORD BUTTONS
    BSF TRISB,2,0
    BSF TRISB,1,0
    BSF TRISB,0,0
    ;SET PWM MOTOR
    BCF TRISE,0,0
    
    RETURN

CLEAR_ALL
    CLRF TRISC,0
    CLRF TRISB,0
    CLRF TRISA,0
    CLRF TRISD,0
    CLRF TRISE,0
    CLRF mode,0
    CLRF debounce_counter,0
    CLRF read_X,0
    CLRF read_Y,0
    CLRF speed,0
    CLRF dir,0
    CLRF delay_L,0
    CLRF delay_H,0
    CLRF alarm_L,0
    CLRF alarm_H,0
    CLRF prev_X,0
    CLRF prev_Y,0
    RETURN

MAIN
    CALL CLEAR_ALL
    CALL INIT_INTERRUPTS
    CALL INIT_PORTS
    CALL INIT_OSC
    BSF mode,0,0
    GOTO MANUAL_MODE
    
MANUAL_MODE
    BCF LATC,4,0
    BSF LATC,5,0
    BSF LATC,6,0

    BTFSC mode,3,0
    CALL CHECK_PRESSED

    BTFSS PORTB,1,0
    GOTO MANUAL_PRESSED

    BTFSS PORTB,0,0
    GOTO RECORD_PRESSED

    GOTO READ_JOYSTICK
    
    GOTO MANUAL_MODE

READ_JOYSTICK
    BCF ADCON0,2,0
    BSF ADCON0,1,0
    CONVERTING_X
    BTFSC ADCON0,1,0
    GOTO CONVERTING_X
    MOVFF ADRESH,read_X
    CLRF dir,0
    BSF ADCON0,2,0
    BSF ADCON0,1,0
    CONVERTING_Y
    BTFSC ADCON0,1,0
    GOTO CONVERTING_Y
    MOVFF ADRESH,read_Y
    CLRF speed,0
    BCF mode,4,0
    
    GOTO CHECK_CENTER

CRUISE_MODE
    BCF LATA,4,0
    BSF LATC,4,0
    BCF LATC,5,0
    BSF LATC,6,0

    BTFSC mode,3,0
    CALL CHECK_PRESSED

    BTFSS PORTB,1,0
    GOTO MANUAL_PRESSED
    
    BSF mode,4,0
    GOTO READ_JOYSTICK
    
    GOTO CRUISE_MODE


MANUAL_PRESSED
    CALL DEBOUNCER

    BTFSC PORTB,1,0; If not pressed, return to previous mode
    GOTO MODE_RETURN
    
    CLRF alarm_L,0
    CLRF alarm_H,0

    BSF mode,3,0; set bit to keep record of pressed button
    ; If button pressed, check if manual or cruise mode
    BTG mode,0,0
    GOTO MODE_RETURN


RECORD_MODE
    BSF LATC,4,0
    BSF LATC,5,0
    BCF LATC,6,0
    
    BTFSC mode,3,0
    CALL CHECK_PRESSED

    BTFSS PORTB,0,0
    GOTO RECORD_PRESSED

    BTFSS PORTB,1,0
    CALL SAVE_DATA
    
    BSF ADCON0,2,0
    BSF mode,4,0
    GOTO CHECK_1MIN


CHECK_1MIN
    MOVLW .11
    SUBWF delay_H,0,0
    BTFSS STATUS,Z,0
    GOTO READ_JOYSTICK
    
    MOVLW .184
    SUBWF delay_L,0,0
    BTFSS STATUS,Z,0
    GOTO READ_JOYSTICK
    
    GOTO CHANGE_RECORD
    
    
SAVE_DATA
    CALL DEBOUNCER
    BTFSC PORTB,1,0
    RETURN
    
    BSF mode,3,0
    BSF FSR0H,0,0
    
    MOVFF read_X,POSTINC0
    MOVFF read_Y,POSTINC0
    MOVFF delay_H,POSTINC0
    MOVFF delay_L,POSTINC0
    
    CLRF delay_H,0
    CLRF delay_L,0
    CLRF alarm_L,0
    CLRF alarm_H,0
    
    MOVFF FSR0L,WREG
    ANDLW 0xFC
    MOVWF steps,0
    MOVLW .120
    CPFSEQ steps,0
    RETURN
    
    BCF mode,1,0
    GOTO MODE_RETURN

AUTOMATIC_MODE 
    MOVLW .0
    CPFSEQ steps,0
    GOTO START_AUTOPILOT
    BCF mode,2,0
    GOTO MODE_RETURN
    
    START_AUTOPILOT
    BCF LATC,4,0
    BCF LATC,5,0
    BCF LATC,6,0
    
    BCF mode,4,0
    MOVFF POSTINC0, read_X
    MOVFF POSTINC0, read_Y
    MOVFF POSTINC0, record_H
    MOVFF POSTINC0, record_L
    CLRF speed,0
    CLRF dir,0
    GOTO CHECK_CENTER
    
    
    CHECK_DELAY
    CLRF delay_L,0
    CLRF delay_H,0
    
    CHECK_RECORD_H
    MOVFF delay_H, WREG
    
    SUBWF record_H,0,0
    BTFSS STATUS,Z,0
    GOTO CHECK_RECORD_H
    
    CHECK_RECORD_L
    MOVFF delay_L, WREG
    SUBWF record_L,0,0
    BTFSS STATUS,Z,0
    GOTO CHECK_RECORD_L
    
    MOVFF FSR0L,WREG
    ANDLW 0xFC
    SUBWF steps,0,0
    BTFSS STATUS,Z,0
    GOTO AUTOMATIC_MODE
    BCF mode,2,0
    GOTO MODE_RETURN

RECORD_PRESSED
    CALL DEBOUNCER

    BTFSC PORTB,0,0
    GOTO MODE_RETURN; If not pressed, return to previous mode

    CLRF alarm_L,0
    CLRF alarm_H,0
    
    BSF mode,3,0; set bit to keep record of pressed button

    ; If button pressed, check if automatic or record mode
    BTFSC mode,1,0; check if already in record mode
    GOTO CHANGE_RECORD
    
    CALL COUNTER_1S
    BTFSS PORTB,0,0; If still being pressed, set automatic mode
    BSF mode,2,0
    
    GOTO CHANGE_RECORD
    
CHANGE_RECORD
    CLRF delay_L,0
    CLRF delay_H,0
    
    BTFSC mode,2,0
    GOTO GOAUTOMATIC

    BTG mode,1,0
    CLRF FSR0L,0
    BTFSC mode,1,0
    CLRF steps,0
    GOTO MODE_RETURN
    
    GOAUTOMATIC
    CLRF FSR0L,0
    BSF FSR0H,0,0
    GOTO AUTOMATIC_MODE
    
MODE_RETURN ; Return to previous mode if unsuccessful debouncer
    BTFSC mode,1,0
    GOTO RECORD_MODE

    BTFSS mode,0,0
    GOTO CRUISE_MODE

    BTFSC mode,0,0
    GOTO MANUAL_MODE

CHECK_PRESSED
    BTFSS PORTB,0,0
    GOTO CHECK_PRESSED

    BTFSS PORTB,1,0
    GOTO CHECK_PRESSED

    BCF mode,3,0
    RETURN


DISPLAY_SPEED
    BTFSC mode,5,0
    GOTO ALARM_ON
    
    BTFSC speed,2,0
    BSF LATA,3,0
    BTFSS speed,2,0
    BCF LATA,3,0
    
    ALARM_ON
    BCF speed,2,0
    MOVFF speed, WREG
    ADDLW 0x10
    MOVWF TBLPTRL,0
    CLRF TBLPTRH,0
    CLRF TBLPTRU,0
    
    TBLRD*
    MOVFF TABLAT,LATD
    BTFSC PORTB,3,0
    BSF LATD,7,0 ; refresh the LED of direction that is in PORTD,7
   
    MOVFF prev_Y,WREG
    SUBWF speed,0,0
    BTFSC STATUS,Z,0
    GOTO END_SPEED
    
    CLRF alarm_L,0
    CLRF alarm_H,0
    MOVFF speed,prev_Y
    
    END_SPEED
    BTFSC mode,2,0
    GOTO CHECK_DELAY
    BTFSC mode,1,0
    GOTO RECORD_MODE
    BTFSC mode,0,0
    GOTO MANUAL_MODE
    
    
DISPLAY_DIR
    BTFSC dir,0,0
    GOTO DISP_CENTER
    
    BTFSC dir,1,0
    GOTO RIGHT_TURN
    
    GOTO LEFT_TURN
    
    RIGHT_TURN
    BCF LATB,7,0
    BCF LATB,6,0
    BCF LATB,5,0
    BCF LATB,4,0
    BCF LATB,3,0
    BCF LATD,7,0
    
    GOTO GO_BACK
    
    DISP_CENTER
    BCF LATB,7,0
    BCF LATB,6,0
    BCF LATB,5,0
    BCF LATB,4,0
    BSF LATB,3,0
    BSF LATD,7,0
    BCF LATC,3,0
    BCF LATC,2,0
    BCF LATC,1,0
    BCF LATC,0,0
    
    GOTO GO_BACK
    
    LEFT_TURN
    BCF LATC,3,0
    BCF LATC,2,0
    BCF LATC,1,0
    BCF LATC,0,0
    BCF LATD,7,0
    BCF LATB,3,0
    
    GOTO GO_BACK
    
    GO_BACK
    
    MOVFF prev_X,WREG
    SUBWF dir,0,0
    BTFSC STATUS,Z,0
    GOTO END_DIR
    
    CLRF alarm_L,0
    CLRF alarm_H,0
    MOVFF dir, prev_X
    
    
    END_DIR
    
    BTFSS mode,0,0
    GOTO CRUISE_MODE
    
    BSF mode,4,0
    GOTO CHECK_CENTER
    
    
CHECK_CENTER
    BTFSC mode,4,0
    GOTO CHECK_Y
    
    MOVLW .100
    CPFSGT read_X,0
    GOTO CHECK_LEFT
    
    MOVLW .155
    CPFSLT read_X,0
    GOTO CHECK_RIGHT
    
    
    MOVLW .120
    MOVWF pwmX_L,0
    MOVLW .1
    MOVWF pwmX_H,0
    GOTO DISPDIR
    
    CHECK_Y
    
    MOVLW .100
    CPFSGT read_Y,0
    GOTO CHECK_UP
    
    MOVLW .155
    CPFSLT read_Y,0
    GOTO CHECK_DOWN
    
    MOVLW .0
    MOVWF pwmY_L,0
    MOVLW .0
    MOVWF pwmY_H,0
    BSF speed,1,0
    BSF speed,0,0
    GOTO DISPLAY_SPEED
    
    
    DISPDIR
    BCF dir,2,0
    BCF dir,3,0
    BSF dir,0,0
    GOTO DISPLAY_DIR
    
    
    
CHECK_DOWN
    BSF speed,2,0
    
    MOVLW .188
    CPFSGT read_Y,0
    GOTO DOWN1
    
    MOVLW .221
    CPFSGT read_Y,0
    GOTO DOWN2
    
    MOVLW .170
    MOVWF pwmY_L,0
    MOVLW .12
    MOVWF pwmY_H,0
    BSF speed,1,0
    GOTO DISPLAY_SPEED
    
    DOWN2
    MOVLW .250
    MOVWF pwmY_L,0
    MOVLW .8
    MOVWF pwmY_H,0
    BSF speed,0,0
    GOTO DISPLAY_SPEED
    
    DOWN1
    MOVLW .250
    MOVWF pwmY_L,0
    MOVLW .4
    MOVWF pwmY_H,0
    GOTO DISPLAY_SPEED
    
CHECK_UP
    MOVLW .66
    CPFSLT read_Y,0
    GOTO UP1
    
    MOVLW .33
    CPFSLT read_Y,0
    GOTO UP2
    
    
    MOVLW .170
    MOVWF pwmY_L,0
    MOVLW .12
    MOVWF pwmY_H,0
    BSF speed,1,0
    GOTO DISPLAY_SPEED
    
    UP2
    
    MOVLW .250
    MOVWF pwmY_L,0
    MOVLW .8
    MOVWF pwmY_H,0
    BSF speed,0,0
    GOTO DISPLAY_SPEED
    
    UP1
    MOVLW .250
    MOVWF pwmY_L,0
    MOVLW .4
    MOVWF pwmY_H,0
    GOTO DISPLAY_SPEED
    

CHECK_RIGHT
    BSF dir,1,0
    
    MOVLW .180
    CPFSGT read_X,0
    GOTO RIGHT
    
    MOVLW .205
    CPFSGT read_X,0
    GOTO RIGHT1
    
    MOVLW .230
    CPFSGT read_X,0
    GOTO RIGHT2
    
    BCF dir,2,0
    BCF dir,3,0
    BCF dir,4,0
    BSF dir,5,0
    BSF LATC,0,0
    BCF LATC,1,0
    BCF LATC,2,0
    BCF LATC,3,0
    MOVLW .175
    MOVWF pwmX_L,0
    MOVLW .2
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
   
    RIGHT
    BSF dir,2,0
    BCF dir,3,0
    BCF dir,4,0
    BCF dir,5,0
    BSF LATC,3,0
    BCF LATC,2,0
    BCF LATC,1,0
    BCF LATC,0,0
    MOVLW .215
    MOVWF pwmX_L,0
    MOVLW .1
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    RIGHT1
    BCF dir,2,0
    BSF dir,3,0
    BCF dir,4,0
    BCF dir,5,0
    BCF LATC,0,0
    BCF LATC,1,0
    BSF LATC,2,0
    BCF LATC,3,0
    MOVLW .30
    MOVWF pwmX_L,0
    MOVLW .2
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    RIGHT2
    BCF dir,2,0
    BCF dir,3,0
    BSF dir,4,0
    BCF dir,5,0
    BCF LATC,0,0
    BSF LATC,1,0
    BCF LATC,2,0
    BCF LATC,3,0
    MOVLW .100
    MOVWF pwmX_L,0
    MOVLW .2
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    
    
CHECK_LEFT
    BCF dir,1,0
    
    MOVLW .75
    CPFSLT read_X,0
    GOTO LEFT
    
    MOVLW .50
    CPFSLT read_X,0
    GOTO LEFT1
    
    MOVLW .25
    CPFSLT read_X,0
    GOTO LEFT2
  
    BCF dir,2,0
    BCF dir,3,0
    BCF dir,4,0
    BSF dir,5,0
    BCF LATB,4,0
    BCF LATB,5,0
    BCF LATB,6,0
    BSF LATB,7,0
    MOVLW .110
    MOVWF pwmX_L,0
    MOVLW .0
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    LEFT 
    BSF dir,2,0
    BCF dir,3,0
    BCF dir,4,0
    BCF dir,5,0
    BSF LATB,4,0
    BCF LATB,5,0
    BCF LATB,6,0
    BCF LATB,7,0
    MOVLW .70
    MOVWF pwmX_L,0
    MOVLW .1
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    LEFT1
    BCF dir,2,0
    BSF dir,3,0
    BCF dir,4,0
    BCF dir,5,0
    BCF LATB,4,0
    BSF LATB,5,0
    BCF LATB,6,0
    BCF LATB,7,0
    MOVLW .250
    MOVWF pwmX_L,0
    MOVLW .0
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
    LEFT2
    BCF dir,2,0
    BCF dir,3,0
    BSF dir,4,0
    BCF dir,5,0
    BCF LATB,4,0
    BCF LATB,5,0
    BSF LATB,6,0
    BCF LATB,7,0
    MOVLW .180
    MOVWF pwmX_L,0
    MOVLW .0
    MOVWF pwmX_H,0
    GOTO DISPLAY_DIR
    
DEBOUNCER
    CLRF aux5,0 
    CLRF aux6,0 
    INNER_BOUNCE_LOOP
    INCF aux5,1,0
    BTFSC STATUS,DC,0	 
    INCF aux6,1,0  
    MOVLW .199 
    SUBWF aux6,0,0
    BTFSC STATUS,Z,0 
      RETURN

    GOTO INNER_BOUNCE_LOOP
    
    
COUNTER_1S 
    CLRF debounce_counter,0
    START_1S
    MOVLW .50
    CPFSEQ debounce_counter,0
    GOTO START_1S
    RETURN
END