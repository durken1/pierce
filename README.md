# Work in progress

# pierce 2.0

![pierce](https://i.imgur.com/JOUH1UI.jpeg "pierce")  

pierce 2.0 is a split modular wireless / semi-wireless 36 key column staggered keyboard. The design is inspired by [Corne](https://github.com/foostan/crkbd) with more pinky stagger as well as only using 1u keycaps.
It is possible to build a wireless version or a semi wireless version. If building the wireless version an additional reciever dongle is required. 
In the semi wireless version the wired half is also acting as a reciever for the wireless half.
Both versions have full [QMK support](https://github.com/qmk/qmk_firmware/tree/master/keyboards/pierce).  
Supported switches: MX, Choc V1, Alps

The semi wireless version has a built in reset circuit for connecting a PS/2 pointing device.

There are 3 different PCBs: wireless (reversible), wired (right) and reciever.

A programmer is necessary to flash the wireless modules. A cheap option is getting an ST-Link V2 clone from ebay.

# Bill of materials
## Wireless
| Qty | Item                                          | Comments                                            |
|----:|-----------------------------------------------|-----------------------------------------------------|
|   2 | Pierce 2.0 wireless PCB                       |                                                     |
|   1 | Pierce 2.0 reciever PCB                       |                                                     |
|   1 | Pro micro or equivalent                       |                                                     |
|   2 | Keystone 3034 Coin cell battery retainer      | SMD                                                 |
|   2 | 2032 3.3V Coin cell battery                   |                                                     |
|   3 | YJ-14015 / Core51822 (B)                      |                                                     |
|   3 | 1x4 pin Right-angle 2.54 mm header            | For programming RF modulules. Optional, temporary soldered wires works fine|
|   2 | 4.7 kΩ resistor                               | SMD 1206                                            |
|   1 | 1117 3.3 V regulator                          | SMD SOT-223                                         |
|   1 | THT momentary switch                          | 2 pin THT 6 mm                                      |
  
## Semi wireless
| Qty | Item                                          | Comments                                            |
|----:|-----------------------------------------------|-----------------------------------------------------|
|   1 | Pierce 2.0 wireless PCB                       |                                                     |
|   1 | Pierce 2.0 wired PCB                          |                                                     |
|   1 | Pro micro or equivalent                       | Elite C recommended if using trackpoint. (Pin D5 broken out)|
|   1 | Keystone 3034 Coin cell battery retainer      | SMD                                                 |
|   1 | 2032 3.3V Coin cell battery                   |                                                     |
|   2 | YJ-14015 / Core51822 (B)                      |                                                     |
|   2 | 1x4 pin Right-angle 2.54 mm header            | For programming RF modulules. Optional, temporary soldered wires works fine|
| 2/4 | 4.7 kΩ resistor                               | SMD 1206, 4 if using trackpoint, otherwise 2        |
|   1 | 1117 3.3 V regulator                          | SMD SOT-223                                         |
|   1 | THT momentary switch                          | 2 pin THT 6 mm                                      |
|   1 | 100 kΩ resistor                               | SMD 1206 Only for trackpoint                        |
|   1 | 2.2 uF capacitor                              | SMD 1206 Only for trackpoint                        |
|   1 | Trackpoint module                             | [R61](https://deskthority.net/wiki/TrackPoint_Hardware#ThinkPad_R61_TrackPoint) recommended, Only for.. well..|
  
# Build instructions
## Soldering

The soldering is pretty straight forward and it should be obvious what component goes where. If you haven't soldered SMD components before I recommend watching a youtube tutorial or two beforehand. I suggest adding a bit of solder to the middle pad for the battery holder.

## Flashing wireless modules

The following instructions is for using an ST-Link V2 clone and Windows.
1. Download and install OpenOCD from https://openocd.org/ and drivers for the programmer from https://www.st.com/en/development-tools/stsw-link009.html
2. Create a file called 'openocd.cfg' in the bin folder where you installed OpenOCD with the following content:

        #nRF51822 Target
        source [find interface/stlink-v2.cfg]

        transport select hla_swd

        set WORKAREASIZE 0x4000
        source [find target/nrf51.cfg]

3. Copy the relevant firmware .bin file to the bin folder where you installed OpenOCD
4. Connect the 4 pins SWCLK, SWDIO, GND and 3.3 V on the ST-Link V2 to the matching pad on the PCB and the ST-Link to the computer
5. Open a command prompt, navigate to the bin folder mentioned above and enter:

        openocd.exe -f openocd.cfg
        
6. Open another command prompt and enter:

        telnet localhost 4444
You should now be connected to the programmer. Enter the following commands to flash the firmware:

    halt
    nrf51 mass_erase
    program <firmware_file_name>.bin
    reset

That's it! You should now have a working pierce keyboard.
