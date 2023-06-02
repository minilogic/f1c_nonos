#ifndef USB_MSC_H
#define USB_MSC_H

/* SCSI Command Operation Code */
#define TEST_UNIT_READY 0x00
#define REQUEST_SENSE   0x03
#define INQUIRY         0x12
#define MODE_SELECT6    0x15
#define MODE_SENSE6     0x1A
#define START_STOP_UNIT 0x1B
#define MEDIUM_REMOVAL  0x1E
#define RD_CAPACITIES   0x23
#define RD_CAPACITY     0x25
#define RD10            0x28
#define WR10            0x2A

/* Read(10) & Write(10) Commands */
typedef struct PACKED {
  u8  opcode;
  u8  rsv1;
  u32 lba;
  u8  rsv2;
  u16 cnt;
  u8  ctrl;
} RD10_CMD, WR10_CMD;

/* Inquiry Command */
typedef struct PACKED {
  u8  opcode;
  u8  rsv1;
  u8  page_code;
  u8  rsv2;
  u8  alen;
  u8  ctrl;
} INQUIRY_CMD;

/* Inquiry Response */
typedef struct PACKED {
  u8  type;
  u8  rmb;
  u8  version;
  u8  format;
  u8  length;
  u8  sccs;
  u8  rsv1;
  u8  rsv2;
  u8  vid[8];
  u8  pid[16];
  u8  rev[4];
} INQUIRY_RES;

/* Read Format Capacity Command */
typedef struct PACKED {
  u8  opcode;
  u8  rsv1[6];
  u16 alen;
  u8  ctrl;
} RD_CAPACITIES_CMD;

/* Read Format Capacity Response */
typedef struct PACKED {
  u32 list_len;   // upper byte must be 8*n (length of formattable capacity descriptor)
  u32 block_num;  // Number of Logical Blocks
  u16 dsc_type;   // 0-reserved, 1-unformatted media, 2-formatted media, 3-no media present
  u16 block_size;
} RD_CAPACITIES_RES;

/* Read Capacity(10) Command */
typedef struct PACKED {
  u8  opcode;
  u8  rsv1;
  u32 lba;
  u16 rsv2;
  u8  pmi;
  u8  ctrl;
} RD_CAPACITY_CMD;

/* Read Capacity(10) Response */
typedef struct PACKED {
  u32 last_lba;   // The last Logical Block Address of the device
  u32 block_size; // Block size in bytes
} RD_CAPACITY_RES;

/* Request Sense Command */
typedef struct PACKED {
  u8  opcode;
  u8  desc;
  u16 rsv1;
  u8  alen;
  u8  ctrl;
  u8  rsv2[6];
} REQUEST_SENSE_CMD;

/* Request Sense Response */
typedef struct PACKED {
  u8  res_code;
  u8  rsv1;
  u8  sense_key;
  u32 info;
  u8  sense_len;
  u32 cmd_info;
  u8  sense_code;
  u8  sense_qualifier;
  u8  unit_code;
  u8  sense_key_spec[3];
} REQUEST_SENSE_RES;

/* Mode Sense(6) Command */
typedef struct PACKED {
  u8  opcode;
  u8  dbd;
  u8  page;
  u8  subpage;
  u8  alen;
  u8  ctrl;
} MODE_SENSE6_CMD;

/* Mode Sense(6) Response */
typedef struct PACKED {
  u8  data_len;
  u8  medium_type;
  u8  param;
  u8  bdsc_len;
} MODE_SENSE6_RES;

#define CBW_SIGNATURE   0x43425355
#define CSW_SIGNATURE   0x53425355

/* Command Block Wrapper */
typedef struct PACKED {
  u32 signature;
  u32 tag;
  u32 total_bytes;
  u8  dir;
  u8  lun;
  u8  cmd_len;
  union {
    u8  raw[16];
    RD10_CMD          rd10;
    WR10_CMD          wr10;
    INQUIRY_CMD       inquiry;
    RD_CAPACITY_CMD   rd_capacity;
    RD_CAPACITIES_CMD rd_capacities;
    REQUEST_SENSE_CMD request_sense;
  } cb;
} CBW;

/* Command Status Wrapper */
typedef struct PACKED {
  u32 signature;
  u32 tag;
  u32 data_residue;
  u8  status;
} CSW;

#endif