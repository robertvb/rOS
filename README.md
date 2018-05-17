The Raspberry Pi is a hardware environment that despite being affordable, it can work as a computer. Its size is less than a hand’s palm but it has an ARM processor running above one Gigahertz. It also has a RISC architecture of 32 bits, a vast RAM capacity and a lot of ports as HDMI video controller, USB ports, SD card driver, Ethernet or uart. In addition, it owns a pins interface of I/O that are configurable.

Linux (Raspbian) is one of the available operating systems that are supported by Raspberry Pi. This software provides the possibility to work with a Raspberry Pi like a home computer because of its multimedia features.

However, all available operating systems like Linux are really complex, so if we want to use it to teach to students it is not going to work very well. Young students are not able to understand how this OS work because they are too complex, and they will get lost trying to link the basic concepts that they know with the new knowledge that they are trying to acquire.

That is the reason why it would be very useful the creation of a simple operating system for Raspberry Pi, although with teaching purpose. In this way this OS could be studied by owners of this small computer which Price is under 40 dollars.

Obviously, this OS should be focused on the memory and process administration, simplifying the calls to the POSIX system and the access to executable files, as well.

Furthermore, the OS is totally self-sufficient, as it includes the basic drivers so it can work, and making it possible that the user does not need a BIOS (unavailable in RPI) and making possible that students can understand the communication with the hardware in deep detail. These drivers are: GPIO, uart, screen, timer and disk.

Every code line related to any action in the system can be checked, so there is no need for dependencies or black boxes. It is a totally new operating system developed from scratch.
