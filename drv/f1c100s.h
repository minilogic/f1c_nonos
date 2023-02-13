#ifndef F1C100S_H
#define F1C100S_H

#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define s8  int8_t
#define s16 int16_t
#define s32 int32_t

#define __I volatile const
#define __O volatile
#define __IO volatile

#define OK 0
#define KO ~OK

#define PACKED __attribute__ ((packed))

/* SYS */
typedef struct {
  __IO  u32 CTRL[16];       // 0x00 SRAM Control registers
} SYS_T;
#define SYS ((SYS_T*)0x01C00000)

/* DRAM CONTROLLER */
typedef struct {
  __IO  u32 SCONR;          // 0x00
  __IO  u32 STMG[2];        // 0x04..0x08
  __IO  u32 SCTLR;          // 0x0C
  __IO  u32 SREFR;          // 0x10
  __IO  u32 SEXTMR;         // 0x14
  __IO  u32 DDLYR;          // 0x24
  __IO  u32 DADRR;          // 0x28
  __IO  u32 DVALR;          // 0x2C
  __IO  u32 DRPTR[4];       // 0x30..0x3C
  __IO  u32 SEFR;           // 0x40
  __IO  u32 MAE;            // 0x44
  __IO  u32 ASPR;           // 0x48
  __IO  u32 SDLY[3];        // 0x4C..0x54
  __IO  u32 MCR[12];        // 0x100..0x12C
  __IO  u32 BWCR;           // 0x140)
} DRAM_T;
#define DRAM ((DRAM_T*)0x01C01000)

/* DRAM PAD */
typedef struct {
  __IO  u32 DRV;            // 0x00 DRAM Pad Multi-Driving Register
  __IO  u32 PUL;            // 0x04 DRAM Pad Pull Register
} DRAM_PAD_T;
#define DRAM_PAD ((DRAM_PAD_T*)0x01C20AC0)

/* CCU */
typedef struct {
  __IO  u32 PLL_CPU_CTRL;   // 0x00 PLL_CPU Control Register
  __IO  u32 RES0;
  __IO  u32 PLL_AUDIO_CTRL; // 0x08 PLL_AUDIO Control Register
  __IO  u32 RES1;
  __IO  u32 PLL_VIDEO_CTRL; // 0x10 PLL_VIDEO Control Register
  __IO  u32 RES2;
  __IO  u32 PLL_VE_CTRL;    // 0x18 PLL_VE Control Register
  __IO  u32 RES3;
  __IO  u32 PLL_DDR_CTRL;   // 0x20 PLL_DDR Control Register
  __IO  u32 RES4;
  __IO  u32 PLL_PERIPH_CTRL;// 0x28 PLL_PERIPH Control Register
  __IO  u32 RES5[9];
  __IO  u32 CPU_CLK_SRC;    // 0x50 CPU Clock Source Register
  __IO  u32 AHB_APB_CFG;    // 0x54 AHB/APB/HCLKC Configuration Register
  __IO  u32 RES6[2];
  __IO  u32 BUS_CLK_GATING0;// 0x60 Bus Clock Gating Register 0
  __IO  u32 BUS_CLK_GATING1;// 0x64 Bus Clock Gating Register 1
  __IO  u32 BUS_CLK_GATING2;// 0x68 Bus Clock Gating Register 2
  __IO  u32 RES7[7];
  __IO  u32 SDMMC0_CLK;     // 0x88 SDMMC0 Clock Register
  __IO  u32 SDMMC1_CLK;     // 0x8C SDMMC0 Clock Register
  __IO  u32 RES8[8];
  __IO  u32 DAUDIO_CLK;     // 0xB0 DAUDIO Clock Register
  __IO  u32 OWA_CLK;        // 0xB4 OWA Clock Register
  __IO  u32 CIR_CLK;        // 0xB8 CIR Clock Register
  __IO  u32 RES9[4];
  __IO  u32 USBPHY_CFG;     // 0xCC USBPHY Clock Register
  __IO  u32 RES10[12];
  __IO  u32 DRAM_GATING;    // 0x100 DRAM GATING Register
  __IO  u32 BE_CLK;         // 0x104 BE Clock Register
  __IO  u32 RES11;
  __IO  u32 FE_CLK;         // 0x10C FE Clock Register
  __IO  u32 RES12[2];
  __IO  u32 TCON_CLK;       // 0x118 TCON Clock Register
  __IO  u32 DI_CLK;         // 0x11C De-interlacer Clock Register
  __IO  u32 TVE_CLK;        // 0x120 TVE Clock Register
  __IO  u32 TVD_CLK;        // 0x124 TVD Clock Register
  __IO  u32 RES13[3];
  __IO  u32 CSI_CLK;        // 0x134 CSI Clock Register
  __IO  u32 RES14;
  __IO  u32 VE_CLK;         // 0x13C VE Clock Register
  __IO  u32 AC_DIG_CLK;     // 0x140 Audio Codec Clock Register
  __IO  u32 AVS_CLK;        // 0x144 AVS Clock Register
  __IO  u32 RES15[46];
  __IO  u32 PLL_STABLE0;    // 0x200 PLL stable time 0
  __IO  u32 PLL_STABLE1;    // 0x204 PLL stable time 1
  __IO  u32 RES16[6];
  __IO  u32 PLL_CPU_BIAS;   // 0x220 PLL CPU Bias Register
  __IO  u32 PLL_AUDIO_BIAS; // 0x224 PLL Audio Bias Register
  __IO  u32 PLL_VIDEO_BIAS; // 0x228 PLL Video Bias Register
  __IO  u32 PLL_VE_BIAS;    // 0x22c PLL VE Bias Register
  __IO  u32 PLL_DDR_BIAS;   // 0x230 PLL DDR Bias Register
  __IO  u32 PLL_PERIPH_BIAS;// 0x234 PLL PERIPH Bias Register
  __IO  u32 RES17[6];
  __IO  u32 PLL_CPU_TUNING; // 0x250 PLL CPU Tuning Register
  __IO  u32 RES18[3];
  __IO  u32 PLL_DDR_TUNING; // 0x260 PLL DDR Tuning Register
  __IO  u32 RES19[8];
  __IO  u32 PLL_AUDIO_PAT;  // 0x284 PLL Audio Pattern Control Register
  __IO  u32 PLL_VIDEO_PAT;  // 0x288 PLL Video Pattern Control Register
  __IO  u32 RES20;
  __IO  u32 PLL_DDR_PAT;    // 0x290 PLL DDR Pattern Control Register
  __IO  u32 RES21[11];
  __IO  u32 BUS_SOFT_RST0;  // 0x2C0 Bus Software Reset Register 0
  __IO  u32 BUS_SOFT_RST1;  // 0x2C4 Bus Software Reset Register 1
  __IO  u32 RES22[2];
  __IO  u32 BUS_SOFT_RST2;  // 0x2D0 Bus Software Reset Register 2
} CCU_T;
#define CCU ((CCU_T*)0x01C20000)

/* DMA */
typedef struct {
  __IO  u32 IE;             // 0x00 DMA Interrupt Enable
  __IO  u32 IS;             // 0x04 DMA Interrupt Status
  __IO  u32 PTY;            // 0x08 DMA Priority
} DMA_T;
#define DMA ((DMA_T*)0x01C02000)

typedef struct {
  __IO  u32 CFG;            // 0x00 Normal DMA Configure
  __IO  u32 SRC;            // 0x04 Normal DMA Source Address
  __IO  u32 DST;            // 0x08 Normal DMA Destination Address
  __IO  u32 CNT;            // 0x0C Normal DMA Byte Counter
} NDMA_T;
#define NDMA0 ((NDMA_T*)0x01C02100)
#define NDMA1 ((NDMA_T*)0x01C02120)
#define NDMA2 ((NDMA_T*)0x01C02140)
#define NDMA3 ((NDMA_T*)0x01C02160)

typedef struct {
  __IO  u32 CFG;            // 0x00 Dedicated DMA Configure
  __IO  u32 SRC;            // 0x04 Dedicated DMA Source Address
  __IO  u32 DST;            // 0x08 Dedicated DMA Destination Address
  __IO  u32 CNT;            // 0x0C Dedicated DMA Byte Counter
  __IO  u32 RES0[2];
  __IO  u32 PAR;            // 0x18 Dedicated DMA Parameter
  __IO  u32 DAT;            // 0x1C Dedicated DMA General Dara (for DDMA3)
} DDMA_T;
#define DDMA0 ((NDMA_T*)0x01C02300)
#define DDMA1 ((NDMA_T*)0x01C02320)
#define DDMA2 ((NDMA_T*)0x01C02340)
#define DDMA3 ((NDMA_T*)0x01C02360)

/* INTC */
enum IRQ_SRCS { IRQ_NMI = 0, IRQ_UART0, IRQ_UART1, IRQ_UART2,
  IRQ_OWA = 5, IRQ_CIR, IRQ_TWI0, IRQ_TWI1, IRQ_TWI2, IRQ_SPI0, IRQ_SPI1,
  IRQ_TIM0 = 13, IRQ_TIM1, IRQ_TIM2, IRQ_WDOG, IRQ_RSB, IRQ_DMA,
  IRQ_TP = 20, IRQ_AUD_CODEC, IRQ_KEYADC, IRQ_SDC0, IRQ_SDC1,
  IRQ_USB = 26, IRQ_TVD, IRQ_TVE, IRQ_TCON, IRQ_DEFE, IRQ_DEBE, IRQ_CSI,
  IRQ_DI, IRQ_VE, IRQ_DAUD, IRQ_PIOD = 38, IRQ_PIOE, IRQ_PIOF };

typedef struct {
  __IO  u32 VECTOR;         // 0x00 Interrupt Vector
  __IO  u32 BASE_ADDR;      // 0x04 Interrupt Base Address
  __IO  u32 RSV;            // 0x08
  __IO  u32 NMI_CTRL;       // 0x0C NMI Interrupt Control
  __IO  u32 PEND[4];        // 0x10 Interrupt IRQ Pending
  __IO  u32 EN[4];          // 0x20 Interrupt Enable
  __IO  u32 MASK[4];        // 0x30 Interrupt Mask
  __IO  u32 RESP[4];        // 0x40 Interrupt Response
  __IO  u32 FF[4];          // 0x50 Interrupt Fast Forcing
  __IO  u32 PRIO[4];        // 0x60 Interrupt Source Priority
} INT_T;
#define INT ((INT_T*)0x01C20400)

/* PIO */
typedef struct {
  __IO  u32 CFG0;           // 0x00 Configure Register 0
  __IO  u32 CFG1;           // 0x04 Configure Register 1
  __IO  u32 CFG2;           // 0x08 Configure Register 2
  __IO  u32 CFG3;           // 0x0C Configure Register 3
  __IO  u32 DAT;            // 0x10 Data Register
  __IO  u32 DRV0;           // 0x14 Multi-Driving Register 0
  __IO  u32 DRV1;           // 0x18 Multi-Driving Register 1
  __IO  u32 PUL0;           // 0x1C Pull Register 0
  __IO  u32 PUL1;           // 0x20 Pull Register 1
} PIO_T;
#define PA  ((PIO_T*)0x01C20800)
#define PB  ((PIO_T*)0x01C20824)
#define PC  ((PIO_T*)0x01C20848)
#define PD  ((PIO_T*)0x01C2086C)
#define PE  ((PIO_T*)0x01C20890)
#define PF  ((PIO_T*)0x01C208B4)

typedef struct {
  __IO  u32 CFG0;           // Interrupt Configure Register 0
  __IO  u32 CFG1;           // Interrupt Configure Register 1
  __IO  u32 CFG2;           // Interrupt Configure Register 2
  __IO  u32 CFG3;           // Interrupt Configure Register 3
  __IO  u32 CTL;            // Interrupt Control Register
  __IO  u32 STA;            // Interrupt Status Register
  __IO  u32 DEB;            // Interrupt Debounce Register
} PINT_T;
#define PD_INT  ((PINT_T*)0x01C20800)
#define PE_INT  ((PINT_T*)0x01C20800)
#define PF_INT  ((PINT_T*)0x01C20800)

/* TIMER */
typedef struct {
  __IO  u32 IE;             // 0x00 IRQ Enable Register
  __IO  u32 IS;             // 0x04 IRQ Status Register
  __IO  u32 RES0[2];
  __IO  u32 T0_CTRL;        // 0x10 Timer 0 Control Register
  __IO  u32 T0_INTV;        // 0x14 Timer 0 Interval Value Register
  __IO  u32 T0_CURV;        // 0x18 Timer 0 Current Value Register
  __IO  u32 RES1;
  __IO  u32 T1_CTRL;        // 0x20 Timer 1 Control Register
  __IO  u32 T1_INTV;        // 0x24 Timer 1 Interval Value Register
  __IO  u32 T1_CURV;        // 0x28 Timer 1 Current Value Register
  __IO  u32 RES2;
  __IO  u32 T2_CTRL;        // 0x30 Timer 2 Control Register
  __IO  u32 T2_INTV;        // 0x34 Timer 2 Interval Value Register
  __IO  u32 T2_CURV;        // 0x38 Timer 2 Current Value Register
  __IO  u32 RES3[17];
  __IO  u32 AVS_CTRL;       // 0x80 AVS Control Register
  __IO  u32 AVS_CNT0;       // 0x84 AVS Counter 0 Register
  __IO  u32 AVS_CNT1;       // 0x88 AVS Counter 1 Register
  __IO  u32 AVS_DIV;        // 0x8C AVS Divisor Register
  __IO  u32 RES4[4];
  __IO  u32 WDOG_IE;        // 0xA0 Watchdog IRQ Enable Register
  __IO  u32 WDOG_IS;        // 0xA4 Watchdog IRQ Status Register
  __IO  u32 RES5[2];
  __IO  u32 WDOG_CTRL;      // 0xB0 Watchdog 0 Control Register
  __IO  u32 WDOG_CFG;       // 0xB4 Watchdog 0 Configuration Register
  __IO  u32 WDOG_MODE;      // 0xB8 Watchdog 0 Mode Register
} TIM_T;
#define TIM ((TIM_T*)0x01C20C00)

/* PWM */
typedef struct {
  __IO  u32 CTRL;           // 0x00 Control Register
  __IO  u32 CH0_PERIOD;     // 0x04 Channel_0 Period Register
  __IO  u32 CH1_PERIOD;     // 0x08 Channel_1 Period Register
} PWM_T;
#define PWM ((PWM_T*)0x01C21000)

/* SPI */
typedef struct {
  __I   u32 RES1;           // 0x000
  __IO  u32 GC;             // 0x004 Global Control Register
  __IO  u32 TC;             // 0x008 Transfer Control Register
  __I   u32 RES2;           // 0x00C
  __IO  u32 IE;             // 0x010 Interrupt Control Register
  __IO  u32 IS;             // 0x014 Interrupt Status Register
  __IO  u32 FC;             // 0x018 FIFO Control Register
  __IO  u32 FS;             // 0x01C FIFO Status Register
  __IO  u32 WC;             // 0x020 Wait Clock Counter Register
  __IO  u32 CC;             // 0x024 Clock Rate Control Register
  __I   u32 RES3[2];        // 0x028..0x2C
  __IO  u32 MBC;            // 0x030 Burst Counter Register
  __IO  u32 MTC;            // 0x034 Transmit Counter Register
  __IO  u32 BCC;            // 0x038 Burst Control Register
  __I   u32 RES4[113];
  union {                   // 0x200 TX Data Register
    __O u8  byte;
    __O u32 word;
  } TX;
  __I   u32 RES5[63];
  union {                   // 0x300 RX Data Register
    __I u8  byte;
    __I u16 sword;
    __I u32 word;
  } RX;
} SPI_T;
#define SPI0 ((SPI_T*)0x01C05000)
#define SPI1 ((SPI_T*)0x01C06000)

/* ADC */
typedef struct {
  __IO  u32 CTRL;           // 0x00 ADC Control
  __IO  u32 INTC;           // 0x04 ADC Interrupt Control
  __IO  u32 INTS;           // 0x08 ADC Interrupt Status
  __IO  u32 DATA;           // 0x0C ADC Data
} ADC_T;
#define ADC ((ADC_T*)0x01C23400)



/* TVE */
typedef struct {
  __IO  u32 ENABLE;         // 0x000
  __IO  u32 CFG1;           // 0x004
  __IO  u32 DAC1;           // 0x008
  __IO  u32 NOTCH_DELAY;    // 0x00C
  __IO  u32 CHROMA_FREQ;    // 0x010
  __IO  u32 FB_PORCH;       // 0x014
  __IO  u32 HD_VS;          // 0x018
  __IO  u32 LINE_NUM;       // 0x01C
  __IO  u32 LEVEL;          // 0x020
  __IO  u32 DAC2;           // 0x024
  __IO  u32 RES0[2];
  __IO  u32 AUTO_EN;        // 0x030
  __IO  u32 AUTO_ISR;       // 0x034
  __IO  u32 AUTO_SR;        // 0x038
  __IO  u32 AUTO_DEB;       // 0x03C
  __IO  u32 CSC1;           // 0x040
  __IO  u32 CSC2;           // 0x044
  __IO  u32 CSC3;           // 0x048
  __IO  u32 CSC4;           // 0x04C
  __IO  u32 RES1[44];
  __IO  u32 CB_RESET;       // 0x100
  __IO  u32 VS_NUM;         // 0x104
  __IO  u32 FILTER;         // 0x108
  __IO  u32 CBCR_LEVEL;     // 0x10C
  __IO  u32 TINT_PHASE;     // 0x110
  __IO  u32 B_WIDTH;        // 0x114
  __IO  u32 CBCR_GAIN;      // 0x118
  __IO  u32 SYNC_LEVEL;     // 0x11C
  __IO  u32 WHITE_LEVEL;    // 0x120
  __IO  u32 ACT_LINE;       // 0x124
  __IO  u32 CHROMA_BW;      // 0x128
  __IO  u32 CFG2;           // 0x12C
  __IO  u32 RESYNC;         // 0x130
  __IO  u32 SLAVE;          // 0x134
  __IO  u32 CFG3;           // 0x138
  __IO  u32 CFG4;           // 0x13C
} TVE_T;
#define TVE ((TVE_T*)0x01C0A000)

/* TCON */
typedef struct {
  __IO  u32 CTRL;           // 0x00 TCON Control
  __IO  u32 INT0;           // 0x04 Interrupt_0
  __IO  u32 INT1;           // 0x08 Interrupt_1
  __IO  u32 RES0;
  __IO  u32 FRM_CTRL;       // 0x10 FRM Control
  __IO  u32 FRM_SEED[6];    // 0x14..0x28 TCON FRM Seed0 RGB, Seed1 RGB
  __IO  u32 FRM_TBL[4];     // 0x2C..0x38 TCON FRM Table 0..3
  __IO  u32 RES1;
  __IO  u32 T0_CTRL;        // 0x40 TCON0 Control
  __IO  u32 T0_CLK;         // 0x44 TCON0 Clock Control
  __IO  u32 T0_TIMING[4];   // 0x48..0x54 TCON0 Basic Timing 0..3
  __IO  u32 T0_HV_TIMING;   // 0x58 TCON0 HV Timing
  __IO  u32 RES2;
  __IO  u32 T0_CPU_IF;      // 0x60 TCON0 CPU Interface Control
  __IO  u32 T0_CPU_WR;      // 0x64 TCON0 CPU Write Data
  __IO  u32 T0_CPU_RD;      // 0x68 TCON0 CPU Read Data
  __IO  u32 T0_CPU_RDNX;    // 0x6C TCON0 CPU Read Back Data
  __IO  u32 RES3[6];
  __IO  u32 T0_IO_CTRL[2];  // 0x88..0x8C TCON0 IO Control 0..1
  __IO  u32 T1_CTRL;        // 0x90 TCON1 Control
  __IO  u32 T1_TIMING[6];   // 0x94..0xA8 TCON1 Basic Timing 0..5
  __IO  u32 RES4[17];
  __IO  u32 T1_IO_CTRL[2];  // 0xF0..0xF4 TCON1 IO Control 0..1
  __IO  u32 RES5;
  __IO  u32 DBG_INFO;       // 0xFC TCON Debug Information
} TCON_T;
#define TCON ((TCON_T*)0x01C0C000)

/* DEFE */
typedef struct {
  __IO  u32 ENABLE;         // 0x000 Module Enable
  __IO  u32 FRM_CTRL;       // 0x004 Frame Process Control
  __IO  u32 BYPASS;         // 0x008 CSC Bypass
  __IO  u32 AGTH_SEL;       // 0x00C Algorithm Selection
  __IO  u32 LINE_INT_CTRL;  // 0x010 Line Interrupt Control
  __IO  u32 res0[3];
  __IO  u32 BUF_ADDR[3];    // 0x020..0x028 Frame Buffer Addresses
  __IO  u32 FIELD_CTRL;     // 0x02C Field Sequence
  __IO  u32 TB_OFFSET[3];   // 0x030..0x038 Tile Based Offsets
  __IO  u32 res1;
  __IO  u32 STRIDE[3];      // 0x040..0x048 Line Strides
  __IO  u32 IN_FMT;         // 0x04C Input Format
  __IO  u32 WB_ADDR;        // 0x050 Write Back Address
  __IO  u32 res2[2];
  __IO  u32 OUT_FMT;        // 0x05C Output Format
  __IO  u32 INT_EN;         // 0x060 Interrupt Enable
  __IO  u32 INT_STAT;       // 0x064 Interrupt Status
  __IO  u32 STAT;           // 0x068 Status
  __IO  u32 res3;
  __IO  u32 CSC_COEF[12];   // 0x070..0x09C CSC Coefficients
  __IO  u32 res4[24];
  __IO  u32 IN_SIZE;        // 0x100 Input Size
  __IO  u32 OUT_SIZE;       // 0x104 Output Size
  __IO  u32 HOR_FACTOR;     // 0x108 Horizontal Factor
  __IO  u32 VER_FACTOR;     // 0x10C Vertical Factor

  __IO  u32 CH0_H_PHASE;    // 0x110 CH0 Horizontal Initial Phase
  __IO  u32 CH0_V_PHASE0;   // 0x114 CH0 Vertical Initial Phase 0
  __IO  u32 CH0_V_PHASE1;   // 0x118 CH0 Vertical Initial Phase 0
  __IO  u32 res5[185];

//  __IO  u32 res5[188];
  __IO  u32 CH0_H_COEF[32]; // 0x400 CH0 Horizontal Filter Coefficient
  __IO  u32 res6[32];
  __IO  u32 CH0_V_COEF[32]; // 0x500 CH0 Vertical Filter Coefficient
  __IO  u32 res7[32];
  __IO  u32 CH1_H_COEF[32]; // 0x600 CH1 Horizontal Filter Coefficient
  __IO  u32 res8[32];
  __IO  u32 CH1_V_COEF[32]; // 0x700 CH1 Vertical Filter Coefficient
} DEFE_T;
#define DEFE ((DEFE_T*)0x01E00000)



/* DEBE */
typedef struct {
  __IO  u32 MODE_CTRL;      // 0x800 Mode Control
  __IO  u32 BACKCOLOR;      // 0x804 Color Control
  __IO  u32 DISP_SIZE;      // 0x808 Disp Size
  __IO  u32 RES0;           // 0x80C
  __IO  u32 LAY_SIZE[4];    // 0x810..0x81C Layer 0..3 Size
  __IO  u32 LAY_POS[4];     // 0x820..0x82C Layer 0..3 Coordinate Control
  __IO  u32 RES1[4];        // 0x830..0x83C
  __IO  u32 LAY_STRIDE[4];  // 0x840..0x84C Layer 0..3 Frame Buffer Line Width
  __IO  u32 LAY_FB_ADDRL[4];// 0x850..0x85c Layer 0..3 Frame Buffer Addr Low
  __IO  u32 LAY_FB_ADDRH[4];// 0x860..0x86c Layer 0..3 Frame Buffer Addr High(4-bits)
  __IO  u32 REG_CTRL;		    // 0x870 Buffer Control Register
  __IO  u32 RES2[3];        // 0x874..0x87C
  __IO  u32 COLOR_KEY_MAX;  // 0x880 Color Key Max
  __IO  u32 COLOR_KEY_MIN;  // 0x884 Color Key Min
  __IO  u32 COLOR_KEY_CFG;  // 0x888 Color Key Configuration
  __IO  u32 RES3;           // 0x88C
  __IO  u32 LAY_ATTR0[4];   // 0x890..0x89C Layer 0..3 Attribute Control 0
  __IO  u32 LAY_ATTR1[4];   // 0x8A0..0x8AC Layer 0..3 Attribute Control 1
  __IO  u32 RES4[10];       // 0x8B0..0x8D4
  __IO  u32 HWC_CTRL;       // 0x8D8 HWC Coordinate Control
  __IO  u32 RES5;           // 0x8DC
  __IO  u32 HWC_FORMAT;     // 0x8E0 HWC Frame Buffer Format
  __IO  u32 RES6[3];        // 0x8E4..0x8EC
  __IO  u32 WB_CTRL;        // 0x8F0 Write Back Control
  __IO  u32 WB_ADDR;        // 0x8F4 Write Back Address
  __IO  u32 WB_STRIDE;      // 0x8F8 Write Back Buffer Line Width
  __IO  u32 RES7[9];        // 0x8FC..0x91C
  __IO  u32 YUV_IN_CTRL;    // 0x920 Input YUV Channel Control
  __IO  u32 RES8[3];        // 0x924..0x92C
  __IO  u32 YUV_ADDR[3];    // 0x930 YUV Channel Frame Buffer Address
  __IO  u32 RES9;           // 0x93C
  __IO  u32 YUV_STRIDE[3];  // 0x940 YUV Channel Buffer Line Width
  __IO  u32 RES10;          // 0x94C
  __IO  u32 COLOR_COEF[12]; // 0x950..0x97C
  __IO  u32 RES11[416];     // 0x980..0x9FC
  __IO  u32 PALETTE[256];   // 0x1000..0x13FC Palette Mode
  __IO  u32 HWC_PATTERN[128]; // 0x1400..0x15FC HWC Pattern Memory Block
  __IO  u32 HWC_PALETTE[64];  // 0x1600..0x163C HWC Palette Table
} DEBE_T;
#define DEBE ((DEBE_T*)0x01E60800)

/* Audio Codec */
typedef struct {
  __IO  u32 DAC_DPC;        // 0x00 DAC Digital Part Control
  __IO  u32 DAC_FIFOC;      // 0x04 DAC FIFO Control
  __IO  u32 DAC_FIFOS;      // 0x08 DAC FIFO Status
  __O   u32 DAC_TXDATA;     // 0x0Ñ DAC TX Data
  __IO  u32 ADC_FIFOC;      // 0x10 ADC FIFO Control
  __IO  u32 ADC_FIFOS;      // 0x14 ADC FIFO Status
  __I   u32 ADC_RXDATA;     // 0x18 ADC RX Data
  __I   u32 RES0;           // 0x1C
  __IO  u32 DAC_MIXER_CTRL; // 0x20 DAC & Out Mixer Control
  __IO  u32 ADC_MIXER_CTRL; // 0x24 ADC & In Mixer Control
  __IO  u32 ADC_DAC_TUNE;   // 0x28 ADC & DAC Performance Tuning
  __IO  u32 CAL_CTRL0;      // 0x2C Bias & DA16 Calibration Control_0
  __I   u32 RES1;           // 0x30
  __IO  u32 CAL_CTRL1;      // 0x34 Bias & DA16 Calibration Control_1
  __I   u32 RES2[2];        // 0x38..0x3C
  __IO  u32 DAC_CNT;        // 0x40 DAC TX FIFO Counter
  __IO  u32 ADC_CNT;        // 0x44 ADC RX FIFO Counter
  __IO  u32 DAC_DBG;        // 0x48 DAC Debug
  __IO  u32 ADC_DBG;        // 0x4C ADC Debug
  __I   u32 RES3[8];        // 0x50..0x6C
  __IO  u32 ADC_DAP_CTRL;   // 0x70 ADC DAP Control
  __IO  u32 ADC_DAP_LCTRL;  // 0x74 ADC DAP Left Control
  __IO  u32 ADC_DAP_RCTRL;  // 0x78 ADC DAP Right Control
  __IO  u32 ADC_DAP_PARA;   // 0x7C ADC DAP Parameters
  __IO  u32 ADC_DAP_LAC;    // 0x80 ADC DAP Left Average Coefficient
  __IO  u32 ADC_DAP_LDAT;   // 0x84 ADC DAP Left Decay & Attack Time
  __IO  u32 ADC_DAP_RAC;    // 0x88 ADC DAP Right Average Coefficient
  __IO  u32 ADC_DAP_RDAT;   // 0x8C ADC DAP Right Decay & Attack Time
  __IO  u32 ADC_DAP_HPFC;   // 0x90 ADC DAP HPF Coefficient
  __IO  u32 ADC_DAP_LINAC;  // 0x94 ADC DAP Left Input Signal Low Average Coef
  __IO  u32 ADC_DAP_RINAC;  // 0x98 ADC DAP Right Input Signal Low Average Coef
  __IO  u32 ADC_DAP_OPT;    // 0x9C ADC DAP Optimum
} AC_T;
#define AC ((AC_T*)0x01C23C00)

/* UART */
typedef struct {
  union {
  __I   u32 RBR;            // 0x00 Receive Buffer Register
  __O   u32 THR;            // 0x00 Transmit Holding Register
  __IO  u32 DLL;            // 0x00 Divisor Latch Low Register
  };
  union {
  __IO  u32 IER;            // 0x04 Interrupt Enable Register
  __IO  u32 DLH;            // 0x04 Divisor Latch High Register
  };
  union {
  __I   u32 IIR;            // 0x08 Interrupt Identify Register
  __O   u32 FCR;            // 0x08 FIFO Control Register
  };
  __IO  u32 LCR;            // 0x0C Line Control Register
  __IO  u32 MCR;            // 0x10 Modem Control Register
  __I   u32 LSR;            // 0x14 Line Status Register
  __I   u32 MSR;            // 0x18 Modem Status Register
  __I   u32 SCH;            // 0x1C Scratch Register
  __I   u32 RES1[23];
  __I   u32 USR;            // 0x7C Status register
  __I   u32 TFL;            // 0x80 Transmit FIFO Level Register
  __I   u32 RFL;            // 0x84 Receive FIFO Level Register
  __I   u32 RES2[7];
  __I   u32 HALT;           // 0xA4 Halt TX Register
} UART_T;
#define UART0 ((UART_T*)0x01C25000)
#define UART1 ((UART_T*)0x01C25400)
#define UART2 ((UART_T*)0x01C25800)

/* TWI */
typedef struct {
  __IO  u32 ADDR;           // 0x00 Slave Address
  __IO  u32 XADDR;          // 0x04 Extended Slave Address
  __IO  u32 DATA;           // 0x08 Data Byte
  __IO  u32 CTRL;           // 0x0C Control
  __IO  u32 STAT;           // 0x10 Status
  __IO  u32 CCR;            // 0x14 Clock Control
  __IO  u32 SRST;           // 0x18 Software Reset
  __IO  u32 EFR;            // 0x1C Enhance Feature Register
  __IO  u32 LCR;            // 0x20 Line Control Register
} TWI_T;
#define TWI0 ((TWI_T*)0x01C27000)
#define TWI1 ((TWI_T*)0x01C27400)
#define TWI2 ((TWI_T*)0x01C27800)

/* SD */
typedef struct {
  __IO  u32 GCTL;           // 0x00 Control Register
  __IO  u32 CKC;            // 0x04 Clock Control Register
  __IO  u32 TMO;            // 0x08 Time Out Register
  __IO  u32 BWD;            // 0x0C Bus Width Register
  __IO  u32 BKS;            // 0x10 Block size Register
  __IO  u32 BYC;            // 0x14 Byte Count Register
  __IO  u32 CMD;            // 0x18 Command Register
  __IO  u32 ARG;            // 0x1C Command Argument Register
  __IO  u32 RESP0;          // 0x20 Response Register 0
  __IO  u32 RESP1;          // 0x24 Response Register 1
  __IO  u32 RESP2;          // 0x28 Response Register 2
  __IO  u32 RESP3;          // 0x2C Response Register 3
  __IO  u32 IMK;            // 0x30 Interrupt Mask Register
  __IO  u32 MIS;            // 0x34 Masked Interrupt Status Register
  __IO  u32 RIS;            // 0x38 Raw Interrupt Status Register
  __I   u32 STA;            // 0x3C Status Register
  __IO  u32 FWL;            // 0x40 FIFO Water Level Register
  __IO  u32 FUNS;           // 0x44 FIFO Function Select Register
  __IO  u32 RES0[4];
  __IO  u32 A12A;           // 0x58 Auto Command 12 Argument Register
  __IO  u32 NTSR;           // 0x5C
  __IO  u32 RES1[6];
  __IO  u32 HWRST;          // 0x78 Hardware Reset Register
  __IO  u32 RES2;
  __IO  u32 DMAC;           // 0x80 BUS Mode Control Register
  __IO  u32 DLBA;           // 0x84 Descriptor List Base Address Register
  __IO  u32 IDST;           // 0x88 DMAC Status Register
  __IO  u32 IDIE;           // 0x8C DMAC Interrupt Enable Register
  __IO  u32 RES3[28];
  __IO  u32 THLDC;          // 0x100 Card Threshold Control Register
  __IO  u32 RES4[2];
  __IO  u32 DSBD;           // 0x10C EMMC4.5 DDR Start Bit Detection Control
  __IO  u32 RES5[60];
  __IO  u32 FIFO;           // 0x200 FIFO Register
} SD_T;
#define SD0 ((SD_T*)0x01C0F000)
#define SD1 ((SD_T*)0x01C10000)

/* VE */
typedef struct {
  __IO  u32 CTRL;           // 0x00 Sub-Engine & RAM Type Select
  __IO  u32 RST;            // 0x04 Sub-Engines Reset
  __IO  u32 RES[58];
  __IO  u32 VERSION;        // 0xF0 IP Version register
} VE_GEN_T;
#define VE  ((VE_GEN_T*)0x01C0E000)

typedef struct {
  __IO  u32 PHDR;           // 0x00 MPEG12 Picture Header register
  __IO  u32 VOPHDR;         // 0x04 MPEG Video Object Plane Header
  __IO  u32 SIZE;           // 0x08 Frame size in MPEG macroblocks (16x16)
  __IO  u32 FRAME_SIZE;     // 0x0C Frame size in pixels
  __IO  u32 MBA;            // 0x10 MPEG Macro Block Address register
  __IO  u32 CTRL;           // 0x14 MPEG Control Register
  __IO  u32 TRIG;           // 0x18 MPEG Decoding Trigger
  __IO  u32 STATUS;         // 0x1C MACC MPEG Status register
  __IO  u32 TRBTRD_FIELD;   // 0x20 Temporal References (TRB(B-VOP)&TRD)
  __IO  u32 TRBTRD_FRAME;   // 0x24
  __IO  u32 VLD_ADDR;       // 0x28 MPEG Variable Length Decoding Address
  __IO  u32 VLD_OFFSET;     // 0x2C MPEG Variable Length Decoding Offset
  __IO  u32 VLD_LEN;        // 0x30 MPEG Variable Length Decoding Length
  __IO  u32 VLD_END;        // 0x34 MPEG video source buffer end
  __IO  u32 MBH_ADDR;       // 0x38 MBH buffer address
  __IO  u32 DCAC_ADDR;      // 0x3C DCAC Buffer address
  __IO  u32 BLK_OFFSET;     // 0x40 MPEG Block address???
  __IO  u32 NCF_ADDR;       // 0x44 NFC buffer address
  __IO  u32 REC_LUMA;       // 0x48 MPEG Luma reconstruct buffer
  __IO  u32 REC_CHROMA;     // 0x4C MPEG Chroma reconstruct buffer
  __IO  u32 FWD_LUMA;       // 0x50 MPEG Luma forward buffer
  __IO  u32 FWD_CHROMA;     // 0x54 MPEG Chroma forward buffer
  __IO  u32 BACK_LUMA;      // 0x58 MPEG Luma Back buffer
  __IO  u32 BACK_CHROMA;    // 0x5C MPEG Chroma Back buffer
  __IO  u32 RES0[8];
  __IO  u32 IQ_MIN_INPUT;   // 0x80 MPEG Inverse Quantization min input level
  __IO  u32 QP_INPUT;       // 0x84 MPEG Inverse Quantization input level
  __IO  u32 RES1[12];
  __IO  u32 JPEG_SIZE;      // 0xB8 JPEG Size
  __IO  u32 JPEG_MCU;       // 0xBC JPEG Minimum Coded Unit
  __IO  u32 JPEG_RES_INT;   // 0xC0 JPEG Restart Interval
  __IO  u32 RES2[2];
  __IO  u32 ROT_LUMA;       // 0xCC MPEG Rotate-Scale Luma buffer
  __IO  u32 ROT_CHROMA;     // 0xD0 MPEG Rotate-Scale Chroma buffer
  __IO  u32 ROT_SCALE_CTRL; // 0xD4 Control Rotate/Scale Buffe
  __IO  u32 JPEG_MCU_START; // 0xD8 JPEG Macro Cell Unit Start
  __IO  u32 JPEG_MCU_END;   // 0xDC JPEG Macro Cell Unit End
  __IO  u32 RAM_WRITE_PTR;  // 0xE0 Auto incremental pointer for rd/wr VE SRAM
  __IO  u32 RAM_WRITE_DATA; // 0xE4 FIFO Like Data register for wr/rd VE SRAM
} MPEG_T;
#define MPEG ((MPEG_T*)0x01C0E100)

typedef struct {
  __IO  u32 PIC_SIZE;       // 0x00 Source picture size in macroblocks (16x16)
  __IO  u32 PIC_STRIDE;     // 0x04 Source picture stride
  __IO  u32 CTRL;           // 0x08 IRQ Control
  __IO  u32 TRIG;           // 0x0C ISP Trigger
  __IO  u32 RES0[7];
  __IO  u32 SCALER_SIZE;    // 0x2C Scaler frame size/16
  __IO  u32 SCALER_OFFSET_Y;// 0x30 Scaler picture offset for luma
  __IO  u32 SCALER_OFFSET_C;// 0x34 Scaler picture offset for chroma
  __IO  u32 SCALER_FACTOR;  // 0x38 Scaler picture scale factor
  __IO  u32 RES1[13];
  __IO  u32 OUTPUT_LUMA;    // 0x70 Output luma address
  __IO  u32 OUTPUT_CHROMA;  // 0x74 Output chroma address
  __IO  u32 PIC_LUMA;       // 0x78 THUMB WriteBack PHY luma address
  __IO  u32 PIC_CHROMA;     // 0x7C THUMB WriteBack PHY chroma adress
  __IO  u32 RES2[24];
  __IO  u32 SRAM_INDEX;     // 0xE0 VE SRAM Index
  __IO  u32 SRAM_DATA;      // 0xE4 VE SRAM Data
} ISP_T;
#define ISP ((ISP_T*)0x01C0EA00)

typedef struct {
  __IO  u32 PARAM;          // 0x04 JPEG/H264 Different Settings
  __IO  u32 QP;             // 0x08 H264 Quantization Parameters
  __IO  u32 RES0;
  __IO  u32 MOTION_EST;     // 0x10 Motion Estimation Parameters
  __IO  u32 CTRL;           // 0x14 Encoder IRQ Control
  __IO  u32 TRIG;           // 0x18 Encoder Trigger
  __IO  u32 STATUS;         // 0x1C Encoder Busy Status
  __IO  u32 BASIC_BITS;     // 0x20 Encoder Bits Data
  __IO  u32 RES1[11];
  __IO  u32 H264_MAD;       // 0x50 H264 Encoder Mean Absolute Difference
  __IO  u32 H264_RES_BITS;  // 0x54 H264 Encoder Residual Bits
  __IO  u32 H264_HDR_BITS;  // 0x58 H264 Encoder Header Bits
  __IO  u32 H264_STATIST;   // 0x5C H264 Encoder unknown statistical data
  __IO  u32 H264_UNK_BUF;   // 0x60 AVC H264 Encoder Unknown Buffer
  __IO  u32 RES2[7];
  __IO  u32 VLE_ADDR;       // 0x80 AVC Encoder Start Address
  __IO  u32 VLE_END;        // 0x84 AVC Encoder End Address
  __IO  u32 VLE_OFFSET;     // 0x88 AVC Encoder Bit Offset
  __IO  u32 VLE_MAX;        // 0x8C AVC Encoder Maximum Bits
  __IO  u32 VLE_LENGTH;     // 0x90 AVC Encoder Bit Length
  __IO  u32 RES3[3];
  __IO  u32 REF_LUMA;       // 0xA0 Luma Reference Buffer
  __IO  u32 REF_CHROMA;     // 0xA4 Chroma Reference Buffer
  __IO  u32 RES4[2];
  __IO  u32 REC_LUMA;       // 0xB0 Luma Reconstruct Buffer
  __IO  u32 REC_CHROMA;     // 0xB4 Chroma Reconstruct Buffer
  __IO  u32 REF_SLUMA;      // 0xB8 Smaller Luma Reference Buffer
  __IO  u32 REC_SLUMA;      // 0xBC Smaller Luma Reconstruct Buffer
  __IO  u32 MB_INFO;        // 0xC0 Temporary Buffer With Macroblock Information
  __IO  u32 RES5[7];
  __IO  u32 SRAM_INDEX;     // 0xE0 AVC VE SRAM Index
  __IO  u32 SRAM_DATA;      // 0xE4 AVC VE SRAM Data
} AVC_T;
#define AVC ((AVC_T*)0x01C0EB04)



#endif /* __F1C100S_H__ */
