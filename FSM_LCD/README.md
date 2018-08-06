#[STK 600] (http://ww1.microchip.com/downloads/en/DeviceDoc/40001904A.pdf)

#[ATMEGA 2560]   (http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf)


#[DC Brush Motor Drivers(36VMax) BD623xxx Series] (http://www.farnell.com/datasheets/2097985.pdf)

#[Nokia5110 LCD(48 × 84 pixels matrix)]  (https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf)

# MOTORSTYRING

Her er en applikasjon som styrer tre motorer uavhengig mellom to forskjellige posisjoner.

## Getting Started

*   Styring foregår via et terminalprogram. 
*	  Når brukeren sender kommandoen ‘INJECT X’ skal motorX kjøres i et visst antall millisekunder i forover retning
* 	Når brukeren sender kommandoen ‘EJECT X’ skal motorX kjøres i et visst antall millisekunder i revers retning
*   Parametere:
    * 	Hver motor skal ha et sett med parametere (alle er heltall):
        *   Tid den skal kjøre i forover retning (i millisec, lovlig verdier er 0 - 3000): ‘TFWD’
        *   Tid den skal kjøre i revers reting (i millisec, lovlig verdier er 0 - 3000): ‘TREV’
        * 	Hastighet i forover retning (i%, lovlig verdier er 0 - 100): ‘SFWD’
        * 	Hastiget i revers retning (i%, lovlig verdier er 0 - 100): ‘SREV’
    *    Hver parameter skal kunne settes fra commandline. Syntax (med TFWD som eksempel):
        *	  SETx TFWD=y  // Setter TFWD for motor X til y msec
        *	  Hver parameter skal kunne leses fra cmdLine:
        *	  GETx TWFD => Skal returnere ‘TFWD=y’

## Tilstander
*	Hver motor skal ha en tilstand. Denne vil variere mellom ‘EJECTING’,  ‘INSERTING’, ‘EJECTED’,’INSERTED’
*	Når brukeren sender strengen ‘STATEx’ så skal applikasjonen svare med den tilstanden motor X er i.
*	Applikasjonen starter alltid i tilstand ‘EJECTED’

## Returverdier
*	Alle kommandoer som applikasjonen forstår skal returnere ‘200 OK\r\n’
*	Alle kommanoder som applikasjonen ikke forstår skal returnere ‘400 Error\r\n\’

### Prerequisites
STK600 skal kobles til via USB, RS232 og Jtag(valgfritt, hvis en ønsker å debugge) 
For å kunne bruke applikasjonen på STK600 og ATMEGA2560 må man gjøre noen forhåndsinstillinger. ATmega2560 trenger 5v og 8Mhz programerebar klokke, og instilles via
ATMEL Studio 7 slik:

```
Last ned alle filene fra git.
```
```
Åpne prosjektet i Atmel studio
```
```
Tools-->Device Programmer-->Board settings
```
```
"SET Vtarget til 5v og clock generator til 8Mhz"
```
### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Test programmet
Sett DC Power supply til 25V.
```
TRYKK PÅ SW5 INTILL NR PÅ LED REPRESENTERER RIKTIG MOTOR DU ØNSKER Å KJØRE
```
```
HOLD SW6 ELLER SW7 MOTOREN INTIL MOTOREN STÅR I RIKTIG POSISJON (EJECTED POSISJONEN)
```
Du er nå klar til å teste ut terminal delen av applikasjonen.
```
Sett igang med å åpne TeraTerm eller et annet terminalprogram og skriv inn følgene og gjør de rette observasjoenen.
```
INJECT "X"(X står for nr på motoren)
```
*Observer at Motoren rotoeren med klokken i 3 sekunder i middels til lav fart.
*Observer på LCD skjermen at M"X" har blitt oppdatert til "INJECTED"

Tast inn følgende komandoer i terminalen:
```
EJECT X
STATE X
SETX SFWD=80
SETX SREV=80
GETX SFWD
GETX SREV
```
Nå skal motoren rotere mot klokken og respondere med "EJECTED", SFWD=80, SREV=80. Som betyr at du har endret farten i forrover og bakover retning fra 50 til 80.
observerer også at LCD skjermen har oppdatert seg.


### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [ATMEL Studio 7](https://www.microchip.com/webdoc/GUID-ECD8A826-B1DA-44FC-BE0B-5A53418A47BD/index.html?GUID-8F63ECC8-08B9-4CCD-85EF-88D30AC06499) - is the integrated development platform (IDP) for developing and debugging used for this application

* [TeraTerm](https://ttssh2.osdn.jp/index.html.en) -  terminal emulator (communications) program.
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc
