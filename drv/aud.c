#include <stdio.h>
#include "sys.h"

volatile struct AC_STATE ac = { .rate = 0,
  .dac.buf = NULL, .dac.size = 0, .dac.head = 0, .dac.tail = 0,
  .dac.mute = 0, .dac.mute_fmin = 1, .dac.mute_micin = 1, .dac.mute_linein = 1,
  .adc.buf = NULL, .adc.size = 0, .adc.head = 0, .adc.tail = 0,
  .adc.mute_fminl = 1, .adc.mute_fminr = 1, .adc.mute_micin = 1,
  .adc.mute_linein = 1, .adc.mute_mixl = 1, .adc.mute_mixr = 1
};

void aud_handler (void)
{
  while(AC->DAC_FIFOS & 0xFF00)
  {
    AC->DAC_TXDATA = ac.dac.head == ac.dac.tail ? 0 : ac.dac.buf[ac.dac.tail++];
    if(ac.dac.tail == ac.dac.size) ac.dac.tail = 0;
  }
  while(AC->ADC_FIFOS & (1 << 23))
  {
    if(ac.adc.buf == NULL) AC->ADC_RXDATA;
    else ac.adc.buf[ac.adc.head++] = AC->ADC_RXDATA;
    if(ac.adc.head == ac.adc.size) ac.adc.head = 0;
  }
}

void ac_mixer_init (void)
{
  u32 mix;
  if(ac.dac.volume < 0) ac.dac.volume = 0;
  if(ac.dac.volume > 63) ac.dac.volume = 63;
  mix = (3u << 30) | (1 << 15) | ac.dac.volume;
  if(!ac.dac.mute) mix |= (3 << 26);
  if(!ac.dac.mute_fmin) mix |= (3 << 28) | (3 << 24) | (7 << 16) | (7 << 8);
  if(!ac.dac.mute_micin) mix |= (3 << 28) | (3 << 24) | (19 << 16) | (19 << 8);
  if(!ac.dac.mute_linein) mix |= (3 << 28) | (3 << 24) | (11 << 16) | (11 << 8);
  AC->DAC_MIXER_CTRL = mix;
  if(ac.adc.gain < 0) ac.adc.gain = 0;
  if(ac.adc.gain > 7) ac.adc.gain = 7;
  if(ac.adc.gain_micin < 0) ac.adc.gain_micin = 0;
  if(ac.adc.gain_micin > 7) ac.adc.gain_micin = 7;
  if(ac.dac.volume_fmin < 0) ac.dac.volume_fmin = 0;
  if(ac.dac.volume_fmin > 7) ac.dac.volume_fmin = 7;
  if(ac.dac.volume_micin < 0) ac.dac.volume_micin = 0;
  if(ac.dac.volume_micin > 7) ac.dac.volume_micin = 7;
  if(ac.dac.volume_linein < 0) ac.dac.volume_linein = 0;
  if(ac.dac.volume_linein > 7) ac.dac.volume_linein = 7;
  mix = (1u << 31) | (ac.dac.volume_micin << 24) | (ac.dac.volume_linein << 21)|
    (ac.adc.gain << 16) | (1 << 14) | (ac.dac.volume_fmin << 4) | 8 |
    ac.adc.gain_micin;
  if(!ac.adc.mute_micin) mix |= (1 << 13);
  if(!ac.adc.mute_fminl) mix |= (1 << 12);
  if(!ac.adc.mute_fminr) mix |= (1 << 11);
  if(!ac.adc.mute_linein) mix |= (1 << 10);
  if(!ac.adc.mute_mixl) mix |= (1 << 9);
  if(!ac.adc.mute_mixr) mix |= (1 << 8);
  AC->ADC_MIXER_CTRL = mix;
}

static int get_sr (int rate)
{
  if(rate == 96000 || rate == 88200) return 7;
  else if(rate == 192000 || rate == 176400) return  6;
  else if(rate == 8000 || rate == 7350) return  5;
  else if(rate == 12000 || rate == 11025) return  4;
  else if(rate == 16000 || rate == 14700) return  3;
  else if(rate == 24000 || rate == 22050) return  2;
  else if(rate == 32000 || rate == 29400) return  1;
  else return  0;
}

void ac_enable (int rate, int mono)
{
  if(!(CCU->PLL_AUDIO_CTRL & 0x80000000))
  {
    CCU->PLL_AUDIO_CTRL |= (1u << 31);
    CCU->BUS_CLK_GATING2 |= 1;
    CCU->AC_DIG_CLK = 1U << 31;
    CCU->BUS_SOFT_RST2 &= ~1;
    CCU->BUS_SOFT_RST2 |= 1;
    INT->BASE_ADDR = 0;
    INT->MASK[0] &= ~(1 << IRQ_AUD_CODEC);
    INT->EN[0] |= (1 << IRQ_AUD_CODEC);
    IRQ_ENABLE();
  }
  mono = mono == 1 ? 1 : 0;
  if((ac.rate != rate) || (ac.dac.mono != mono))
  {
    ac.rate = rate;
    ac.dac.mono = mono;
    CCU->PLL_AUDIO_CTRL = ac.rate % 1000 ? 0x80004E14 : 0x80005514;
    while(!(CCU->PLL_AUDIO_CTRL & (1U << 28))); // 44.1/48: PLL=22579.2/24576kHz
    AC->DAC_FIFOC = (get_sr(rate) << 29) | ((rate < 32000) << 28) | (1 << 24) |
      (3 << 21) | (15 << 8) | (mono << 6) | (1 << 3) | 1;
    AC->ADC_FIFOC = (get_sr(rate) << 29) | (1 << 28) | (1 << 24) | (7 << 16) |
      (15 << 8) | (1 << 7) | (1 << 3) | 1;
    ac_mixer_init();
    AC->DAC_DPC |= (1u << 31);
    AC->DAC_FIFOS |= 0x0E;
    AC->ADC_FIFOS |= 0x0A;
  }
}

void ac_disable (void)
{
  if(!(CCU->PLL_AUDIO_CTRL & 0x80000000)) return;
  INT->EN[0] &= ~(1 << IRQ_AUD_CODEC);
  INT->MASK[0] |= (1 << IRQ_AUD_CODEC);
  AC->ADC_MIXER_CTRL = 0;
  AC->DAC_MIXER_CTRL = (1 << 15);
  AC->DAC_DPC &= ~(1u << 31);
  CCU->BUS_CLK_GATING2 &= ~1;
  CCU->AC_DIG_CLK = 0;
  CCU->PLL_AUDIO_CTRL &= ~(1u << 31);
  ac.rate = 0;
}
