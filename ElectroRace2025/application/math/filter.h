#ifndef __FILTER_H
#define __FILTER_H

typedef struct
{
 float input[3];
 float output[3];
}lpf_buf;

typedef struct
{
  float a[3];
  float b[3];
}lpf_param;


float LPButterworth(float curr_input,lpf_buf *buf,lpf_param *params);
void set_cutoff_frequency(float sample_frequent, float cutoff_frequent,lpf_param *LPF);


#endif



