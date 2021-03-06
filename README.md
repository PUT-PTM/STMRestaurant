# STMRestaurant

STMRestaurant is a project that can replace a waiter in a restaurant.

### Description

##### Folders

/Coocox.
* C program. Use a microcontroller STM32F407 with a display of nokia 5110, selecting by 4 buttons, connecting with PC by HC-05 bluetooth module.

/WindowsForms restaurant.	
* C# program. Use your PC with bluetooth module and Internet access to receive orders and send it to the server.

### Pins

HC-05 Bluetooth module:

* Tx PB11
* Rx PB10

Buttons:

* down	PB07
* up	PB06
* ok	PB05
* back	PB04

*all buttons connected also to the ground by resistors*

*after pressing, on pin appears high state*

LCD module:

* RST	PC11
* CE	PC9
* DC	PC7
* DIN	PC5
* CLK	PC3
* LED	PC1 /  GND

### IDE

 * [CooCox CoIDE] 
 * [Visual Studio 2013]	
 
### Attributions

* http://stm32f4-discovery.com/pcd8544-nokia-33105110-lcd-stm32f429-discovery-library/

### License
The MIT License ([MIT])

### Future improvements

* Upgrade of database.
* Version of C# application dedicated for certain user.
* New design for whole physical system (STM case).
* Better LCD display.

### Contributors

* Tomasz Polański 
* Karol Winiecki

*The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.*
Supervisor: [Michał Fularz]


[CooCox CoIDE]:http://www.coocox.org

[Michał Fularz]:https://github.com/Michal-Fularz
[Visual Studio 2013]:https://www.visualstudio.com
[MIT]:http://opensource.org/licenses/MIT

