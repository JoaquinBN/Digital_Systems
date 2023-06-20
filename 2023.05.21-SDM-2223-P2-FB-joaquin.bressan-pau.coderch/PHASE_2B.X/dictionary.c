//DICTIONARY


//INTERFACE 1 (AUDIO)
void playMusic(char aux);
//PRE:
//POST: Turns on the audio and starts the music sequence

void stopMusic(void);
//PRE:
//POST: Stops the audio and therefore the music sequence

//INTERFACE 2 (EEPROM)
void saveData(char index, char data);
//Pre: EECON1's WR bit needs to be 0.
//Post: Writes data in the EEPROM at address 'index'

char readData(char index);
//Pre: EECON1's RD bit needs to be 0.
//Post: Reads data from the EEPROM at address 'index'

void displayRecordings(char indexPosition);
//Post: Display the recordings information

char isIdle(void);
//Post: return 1 if the EEPROM motor is in idle state (state = 0) and 0 otherwise

void sendTimestamp(void);
//Pre: Have saved the system's current timestamp in a variable
//Post: Send timestamp to the java interface

void saveIndex(void);
//Pre: Have received a value from the RCREG register
//Post: Write the index received to the EEPROM

void sendIndex(char indexPosition);
//Post: Send index of the record to play to the Java interface

char getNumRecordings(void);
//Post: return the number of recordings. If the indexEEPROM is higher than 8,
//it returns 8. Otherwise, it returns the indexEEPROM's value.

void chargeMemory(void);
//Pre: Have entered the port name
//Post: Write the information from the recordings that was stored in the EEPROM to the flash memory.

//INTERFACE 3 (Joystick)
char getJoystickUp(void);
//PRE:
//POST: Returns 1 in the case that the joystick has been moved up and 0 otherwise

char getJoystickDown(void);
//PRE:
//POST: Returns 1 in the case that the joystick has been moved down and 0 otherwise

void joystickIdle(void);
//Post: Deactivate the joystick in order to allow the recording through the microphone.
//The bits of the analogue channel select bits are set for the microphone port to be read.
//Puts the joystick in IDle mode

void joystickAlive(void);
//Post: Activates the joystick. The bits of the analogue channel select bits are set for the joystick port to be read.




//INTERFACE 4 (Keypad)
char isPressed(void);
//PRE: 
//POST: Returns 1 if any key has been pressed in the keypad and 0 otherwise

char getDigit(void);

char getCharacter(void);
//PRE: isPressed()
//POST: Returns the character that has been pressed

char isNextKey(void);
//PRE: isPressed()
//POST: Returns if another key was pressed or a second had passed.

char getDigitValue(void);
//PRE: isPressed()
//POST: Returns the digit that has been pressed (the number not the character)




//INTERFACE 5 (lcTLCD)
void LcInit(char rows, char columns);
// Pre: Rows = {1, 2, 4}  Columns = {8, 16, 20, 24, 32, 40 }
// Pre: There is a free timer
// Pre: It needs 40ms of tranquility between the VCC raising and this constructor being called.
// Post: This routine can last up to 100ms
// Post: The display remains cleared, the cursor turned off and at the position 0, 0.

void LcEnd(void);
// The destructor

void LcClear(void);
// Post: Clears the display and sets the cursor to its previous state. 
// Post: The next order can last up to 1.6ms. 

void LcCursorOn(void);
// Post: Turn on the cursor
// Post: The next order can last up to 40us. 

void LcCursorOff(void);
// Post: Turns off the cursor
// Post: The next order can last up to 40us. 

void LcGotoXY(char Column, char Row);
// Pre : Column between 0 and 39, row between 0 and 3. 
// Post: Sets the cursor to those coordinates. 
// Post: The next order can last up to 40us. 

void LcPutChar(char c);
// Post: Paints the char in the actual cursor position and increments 
// its position. If the column gets to 39 it returns to 0.
// The row of the LCD is increased when this happens until the second
// row and then it is reset back to row 0 if it has 2 rows total. 
// If the LCD has 4 rows it will reset back to row 0 when it
// reaches row 4 and the columns will go till 39 before reseting to 0.
// The one row LCDs returns to 0 when a column gets to 39. 
// The row is never increased. 
// The char is written

void LcPutString(char *s);
// Post: Paints the string from the actual cursor position. 
// The coordinate criteria is the same as the LcPutChar. 
// Post: Can last up to 40us per char of a routine output.

char getColumnAct(void);
//Post: returns the value of the actual cursor's column




//INTERFACE 6 (SystemTimer)
void updateSysTimestamp(Timestamp time, char i);
//Post: updates the timestamp of the element at index i of SystemTimer array of timestamps.

void incrementSysTimestamp(void);
//Post: increments the timestamp of the system timer (current time) by 1 second.

void displayTimestamp(char i);
//Post: redirects to the specific state to print a specific timestamp in the LCD display at index i of the array of timestamps

char finishedDisplay(void);
//Post:  returns if the system timer motor is in state 0 or displaying a timestamp.

Timestamp getSysTimestamp(void);
//Post: returns the system current timestamp.




//INTERFACE 7 (SIO)
void setTX(unsigned char letter);
//Pre: The TMRT bit of TXSTA register must be 1.
//Post: The parameter letter is assigned to the TXREG.

unsigned char checkTX(void);
//Post: returns the TXSTA's TMRT bit value.

unsigned char checkRX(void);
//Post: returns the value of PIR1's RCIF flag bit.

unsigned char getRX(void);
//Pre: The RCIF bit of PIR1 register must be 1.
//Post: The RCREG is returned corresponding to the received value by the SIORx.




//INTERFACE 8 (Timer)
void RSI_Timer0(void);
    // IMPORTANT! This function needs to be called from the Interrupt Service Routine (ISR) when TMR0IF==1.
    // This ISR must contain: if (TMR0IF==1) RSI_Timer0();

void TI_Init (void);
// Pre: Global interrupts are enabled
// Post: Constructor. It is a global preconidtion to have instantiated this method before using any other function of this ADT

unsigned char TI_NewTimer(unsigned char *TimerHandle) ;
// Post: Returns TI_TRUE when a new virtual timer has been successfully created, it returns TI_FALSE otherwise
// Writes in *TimerHandle the handled of the assigned timer

void TI_ResetTics (unsigned char TimerHandle);
// Pre: TimerHandle comes from Ti_NewTimer.
// Post: Stores the time reference associated to TimerHandle.

unsigned long TI_GetTics (unsigned char TimerHandle);
// Pre: TimerHandle comes from Ti_NewTimer.
// Post: Returns the number of tics that have happened since the last TI_ResetTics of TimerHandle.

void TI_CloseTimer (unsigned char TimerHandle);
// Pre: TimerHandle comes from Ti_NewTimer.
// Post: Releases the virtual timer associated to TimerHandle

void TI_End (void);
// Post: Destructor of the ADT
