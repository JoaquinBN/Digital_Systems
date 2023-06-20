The PHASE_2B.X folder corresponds to the MPLAB project containing the .c and .h files as well as the dictionary.c file.
The LCControlTower.zip contains the Java interface, with the code to be implemented within it.


When the microcontroller is turned on, an access 
menu must be displayed on the LCD screen to 
indicate to which control tower (port identifier) 
do we want to establish a communication. In this 
menu, the user will have to enter the port code 
using the 3x4 matrix keyboard. The port code always consists of three letters (for example: 
Barcelona is BCN, Madrid is MAD, London is LDN, etc.).
To set up the port we want to communicate with, a matrix keypad 
(whose bounce must be filtered by software) will be used in SMS 
format with sound effects. Therefore, each key will generate a sound 
of a different frequency when pressed. Thus, there will be a total of 
12 different sounds. As you know, the SMS format consists of, for 
example, if the user wants to write a 'C', they will have to quickly 
press the '2' key three times. If this key is not pressed again before a 
second or any other key is pressed, the "C" character will be 
considered definitive. On the other hand, if the same key is pressed multiple times, with less than 
one second between two consecutive pressings, the letters associated with that key will cyclically 
increment (for example, "A", "B", "C", "2", "A", "B", ...). It is also important to remember that the 
'0' key rotates from '0' to ' ' (space) constantly. When a key is pressed, the system must display 
the new value of the key through the LCD. It is important to remember the SMS operation with 
the following example:
I press 2, it displays 'A', I press 2 again, it displays 'B', I press 3, it displays 'BD', I press 3 again, it 
displays 'BE'.
If (*) is pressed while entering a code, all characters that have been written up to that point will 
be erased. When (#) is pressed, the character 'T' will be sent to the Java interface, which will 
respond with the character 'K', confirming that it is ready to receive what the user has just written. 
4
LSControlTower – Practice 2 Phase B
Digital Systems and Microcontrollers
Then, the microcontroller will have to send the name of the control tower and finally, the 
character '\0'. The Java interface will begin to display the name of the control tower being 
contacted and then, the main menu of the system will be displayed on the LCD screen, which is 
detailed below.
Also, from the moment a new communication with a port control tower is initiated, time must 
be counted in ascending order. Therefore, the system must start counting the number of 
seconds and minutes (MM:SS) that pass.