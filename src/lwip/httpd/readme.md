# Example of using USB-Ethernet adapter and lwIP-library

Adapters based on the RTL8152B chip are supported. The driver from [u-boot](https://github.com/u-boot/u-boot/tree/master/drivers/usb/eth) was taken as the base. This program runs on bare metal and depends on the [lwIP TCP/IP stack](https://savannah.nongnu.org/projects/lwip/). In this example, the available services are ping, iperf, and http server. At the same time, iperf demonstrated the use of a bandwidth of more than 90Mbits/sec.
