#ifndef AUD_H
#define AUD_H

struct AC_STATE {
  int rate;
  struct {
    s16 *buf;
    int size;
    int head;
    int tail;
    int mono;
    int volume;
    int volume_fmin;
    int volume_micin;
    int volume_linein;
    int mute        : 1;
    int mute_fmin   : 1;
    int mute_micin  : 1;
    int mute_linein : 1;
  } dac;
  struct {
    s16 *buf;
    int size;
    int head;
    int tail;
    int gain;
    int gain_micin;
    int mute_fminl  : 1;
    int mute_fminr  : 1;
    int mute_micin  : 1;
    int mute_linein : 1;
    int mute_mixl   : 1;
    int mute_mixr   : 1;
  } adc;
};

extern volatile struct AC_STATE ac;

void ac_disable (void);
void ac_enable (int rate, int mono);
void ac_mixer_init (void);
void aud_handler (void);

#endif