# Bare metal code examples for Allwinner's F1C100S (F1C200S) SOC

These examples work without any operating system environment. It's like programming simple microcontrollers or "old school" programming techniques. Examples can be used for educational purposes to teach low-level programming. All programs can be run on [f1c_dbc](https://github.com/minilogic/f1c_dbc) computer, [LicheePi Nano](https://linux-sunxi.org/LicheePi_Nano), [MangoPi](https://mangopi.org/mangopi_r) or [CherryPi](https://linux-sunxi.org/Lctech_Pi_F1C200s) demoboards.
Low-level framework is simple, but provides the following features:
- work with graphic display modules: TV, TFT, HDMI (work in progress);
- examples of working with communication interfaces: USB, UART, SPI, TWI(I2C);
- playback and recording with built-in hardware audio codec;
- USB Device/Host driver implementation;
- µSD memory card support;
- bootloader implementation from SPI-flash memory;
- examples of integrating third-party libraries: FatFs, LVGL, LWIP, TinyGL, MP3 Decoder/Encoder...

![intro](https://github.com/minilogic/f1c_nonos/assets/108269914/28806b5c-5cea-4a52-84ff-2061686fc2ee)

**Project directories:**

| Name                             | Description                                |
|----------------------------------|--------------------------------------------|
| [doc](./doc)                     | Documentation                              |
| [drv](./drv)                     | Low-level drivers                          |
| [lib](./lib)                     | Third-party libraries                      |
| [src/demo](./src/demo)           | Classical demo effects                     |
| [src/gui](./src/gui)             | LVGL GUI-demo (LVGL7, LVGL9)               |
| [src/tinygl](./src/tinygl)       | TinyGl 3D-demo examples                    |
| [src/slideshow](./src/slideshow) | µSD & FatFs example (JPG-slideshow)        |
| [src/usbd](./src/usbd)           | USB-device examples                        |
| [src/usbh](./src/usbh)           | USB-host examples                          |
| [src/twi](./src/twi)             | TWI(I2C) examples (RTC, TS)                |
| [src/lwip](./src/lwip/httpd)     | lwIP over USB-Ethernet adapter RTL8152B    |
| [src/audio](./src/audio)         | Audio examples (MP3 Decoder/Encoder)       |
| [src/coremark](./src/coremark)   | CoreMark Benchmark                         |
| [tools/sunxi](./tools/sunxi)     | Tools for loading and flashing the SOC     |
| [tools/zadig](./tools/zadig)     | Windows tool for installing SOC-driver     |
| [tools/iperf](./tools/iperf)     | TCP/IP speed test tool                     |

**Useful links:**
1. [XBOOT](https://github.com/xboot/xboot)
2. [RT-Thread](https://github.com/RT-Thread/rt-thread)
3. [F1C200S Library](https://github.com/lhdjply/f1c200s_library)
4. [F1C100S Projects](https://github.com/nminaylov/F1C100s_projects)
5. [F1C200s and F1C100s study](https://github.com/weimingtom/wmt_f1c_study)
6. [WhyCan Forum](https://whycan.com/f_17.html)

**Disclaimer:**
The Software is furnished "AS IS", without warranty as to performance or results, and the entire risk as to performance or results is assumed by YOU. I disclaims all warranties, express, implied or otherwise, with regard to the Software, its use, or operation, including without limitation any and all warranties of merchantability, fitness for a particular purpose, and non-infringement of intellectual property rights.

