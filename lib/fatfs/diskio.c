/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sys.h"

#define DRIVE_NUM 8
#define SECTOR_SIZE	512

struct {
  volatile DSTATUS stat;
  int (*cbrd) (void *ptr, u32 addr, u32 cnt);
  int (*cbwr) (void *ptr, u32 addr, u32 cnt);
} drv[DRIVE_NUM];

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status ( BYTE pdrv /* Physical drive number */ )
{
  return pdrv < DRIVE_NUM ? drv[pdrv].stat : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize ( BYTE pdrv /* Physical drive number */ )
{
  if(pdrv >= DRIVE_NUM) return STA_NOINIT;
  drv[pdrv].stat &= ~STA_NOINIT;
  return drv[pdrv].stat;
}

void disk_init ( BYTE pdrv, int (*cbrd) (void *ptr, u32 addr, u32 cnt),
  int (*cbwr) (void *ptr, u32 addr, u32 cnt)
)
{
  if(pdrv < DRIVE_NUM)
  {
    drv[pdrv].stat = STA_NOINIT;
    drv[pdrv].cbrd = cbrd;
    drv[pdrv].cbwr = cbwr;
  }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
  BYTE pdrv,        /* Physical drive number to identify the drive */
  BYTE *buff,       /* Data buffer to store read data */
  LBA_t sector,     /* Start sector in LBA */
  UINT count        /* Number of sectors to read */
)
{
  if(pdrv >= DRIVE_NUM || !count) return RES_PARERR;
  if(drv[pdrv].stat & STA_NOINIT) return RES_NOTRDY;
  return drv[pdrv].cbrd((void *)buff, sector, count) == count ? RES_OK : RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write (
  BYTE pdrv,        /* Physical drive nmuber to identify the drive */
  const BYTE *buff, /* Data to be written */
  LBA_t sector,     /* Start sector in LBA */
  UINT count        /* Number of sectors to write */
)
{
  if(pdrv >= DRIVE_NUM || !count) return RES_PARERR;
  if(drv[pdrv].stat & STA_NOINIT) return RES_NOTRDY;
  return drv[pdrv].cbwr((void *)buff, sector, count) == count ? RES_OK : RES_ERROR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
  BYTE pdrv,        /* Physical drive nmuber (0..) */
  BYTE cmd,         /* Control code */
  void *buff        /* Buffer to send/receive control data */
)
{
  switch (cmd)
  {
    case CTRL_SYNC: return RES_OK;
    case GET_SECTOR_SIZE:
      *(DWORD *) buff = (DWORD) SECTOR_SIZE;
      return RES_OK;
    case GET_BLOCK_SIZE:
      *(DWORD *) buff = (DWORD) SECTOR_SIZE;
      return RES_OK;
    default: return RES_PARERR;
  }
  return RES_OK;
}
