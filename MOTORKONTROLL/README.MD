# MOTORCONTROLL
This is an application which can controll tre DC-motors individualy and change between to diffrent positions.


## Getting Started
*  This is a termainal program and all commands take effect through the terminal.
*  When the user writes the command ‘INJECT X’ will motorX rotate in a forward direction in a spesific amount of milisec. 
*  When the user writes the command ‘EJECT X’ will motorX rotate in a reverse direction in a spesific amount of milisec.
*   Parameters:
    *   Evrey motor has a sett with parameters(all are integers):
        *   Time the motor will run forward(in milisec, 0-3000): ‘TFWD’
        *   Time the motor will run reverse(in milisec, 0-3000): ‘TREV’
        * 	Speed in forward direction(in%,  0 - 100): ‘SFWD’
        * 	Speed in revese direction (in%,  0 - 100): ‘SREV’
    *   Evrey parameter is customizeble in the commandline. Syntax(with TFWD as eksample):
        *	  SETx TFWD=y  // will set TFWD for motor X to y msec
        *	  Evrey parameter is readable from the cmdLine:
        *	  GETx TWFD => Will return ‘TFWD=y’

## States
* Evrey motor have 4 states. It can vary between ‘EJECTING’,  ‘INSERTING’, ‘EJECTED’,’INSERTED’.
* When the user types string ‘STATEX’, will the application reply with the state motor X is in.
* Application allways startes in state ‘EJECTED’.

## Retur values
*	All commands which the application accepts will return ‘200 OK\r\n’.
*	All commands which the application dows not accepts will return ‘400 Error\r\n\’.

## Terminal
* Evreything that is sendt to the terminal will be eccoed back.
* Backspace works as inteneded.
* The Delete-button deletes the current string.


### Prerequisites
STK must be powered through the USB connection. For the application to work the UART connection though RS232 must be connected(Jtag is optional for debugging).
In additon the user must change some settings on the STK600 to power the ATMEGA2560 and use the correct frequancy. ATmega2560 uses 5v og 8Mhz programable clock, and can be sett from ATMEL Studio 7:

```
Download all files from git.
```
```
Open the prosject in Atmel studio
```
```
Tools-->Device Programmer
```
```
From the pulldown meny select "STK600", Apply--->Read-->Board settings
```
```
"SET Vtarget to 5v og clock generator to 8Mhz"
```
### Installing
This is a step by step series of exsamples which will explain the user how to set up the application. By following the instructions, which the pictures bellow ilustrates, can the user connect the LCD, motor, and generel interconnections on the stk600(Datashhet for more information)
#### LCD connections(Nokia5110):
![LCD connections](https://github.com/HenriknWold/WindowMote/blob/master/MOTORKONTROLL/LCD_NOKIA.PNG)
#### Interconnections(STK600):
![INTERCONNECTIONS connections](https://github.com/HenriknWold/WindowMote/blob/master/MOTORKONTROLL/INTERCONNECTIONS.PNG)
#### MotorControll(BD6230F):
![MOTORKONTROLL](https://github.com/HenriknWold/WindowMote/blob/master/MOTORKONTROLL/MOTOR.PNG)
*  Make sure that the there is a common connection to ground.
*  Make sure that STK600 is preconnected correctly(Datasheet stk600)



## Test the application
Sett DC Power supply to 10-25V.(25 optimal)
```
PRESS SW5 UNTIL NUMBER ON THE LEDLIGHT REPRESENTATES THE RIGHT MOTOR THE USER WHATS TO RUN MANUALY.
```
```
HOLD SW6 OR SW7 UNTIL THE MOTOREN IS MOVED TO THE CORRECT POSITON(EJECTED POSITION)
```
The user is now ready to test the terminal application.

Start with opening TreaTerm or any other terminal program. Whrite the following command and the the correct observations.
```
INJECT "X"(X stands for the motor number )
```
*Observe that the motor rotates with the clock for 3seconds at a midium velocity.
*Observe on the LCD screen that the state of MX has been updated to "INSERTED".

For further testing type the following cmd:
```
EJECT X
STATE X
SETX SFWD=80
SETX SREV=80
GETX SFWD
GETX SREV
```
Now the motor should rotate aganst the clock and respond in the terminal window "EJECTED", SFWD=80, SREV=80. this means that the user has changed the speed of the motor in forward and revers motion from 50% to 80%. In additon the user can observe that the LCD has updated again.



## Datasheet
*  [STK 600](http://ww1.microchip.com/downloads/en/DeviceDoc/40001904A.pdf)
*  [ATMEGA 2560](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf)
*  [DC Brush Motor Drivers(36VMax) BD623xxx Series](http://www.farnell.com/datasheets/2097985.pdf)
*  [Nokia5110 LCD(48 × 84 pixels matrix)](https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf)



## Built With

* [ATMEL Studio 7](https://www.microchip.com/webdoc/GUID-ECD8A826-B1DA-44FC-BE0B-5A53418A47BD/index.html?GUID-8F63ECC8-08B9-4CCD-85EF-88D30AC06499) - is the integrated development platform (IDP) for developing and debugging used for this application

* [TeraTerm](https://ttssh2.osdn.jp/index.html.en) -  terminal emulator (communications) program.



