#include <stdio.h>
#include "sys.h"
#include "usbh.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/lwiperf.h"

u8 ip_addr[4] = { 192, 168, 1, 191 };
u8 ip_mask[4] = { 255, 255, 255, 0 };
u8 ip_gate[4] = { 192, 168, 1, 1 };
u8 ip_mac[6]  = { 0xF8, 0xF0, 0x12, 0x34, 0x00, 0x00 };

void lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port,
  const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(local_addr);
  LWIP_UNUSED_ARG(local_port);
  printf("IPERF%c %s:%d %"U32_F"(bytes) %"U32_F"(ms) %"U32_F"(kbits/sec)\n",
    report_type ? 'C' : 'S', ipaddr_ntoa(remote_addr), (int)remote_port,
    bytes_transferred, ms_duration, bandwidth_kbitpsec);
}

int main (void)
{
  puts("\e[36mF1C100S USBH & RTL8152B & LWIP-"LWIP_VERSION_STRING"\e[0m");
  usb_mux(USB_MUX_HOST);
  usbh_init();
  lwip_init();
  httpd_init();
  lwiperf_start_tcp_server_default(lwiperf_report, NULL);
  while(1)
  {
    usbh_handler();
    sys_check_timeouts();
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
