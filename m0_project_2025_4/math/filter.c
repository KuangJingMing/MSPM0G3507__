#include "stdint.h"
#include "filter.h"
#include "math.h"
#include "wp_math.h"
/***************************************************
函数名: float LPButterworth(float curr_input,lpf_buf *buf,lpf_param *params)
说明:	二阶巴特沃斯低通滤波器
入口:	float curr_input-当前滤波器输入
			lpf_buf *buf-滤波器中间状态
			lpf_param *params-滤波器参数
出口:	float 滤波器输出值
备注:	无
作者:	无名创新        
****************************************************/
float LPButterworth(float curr_input,lpf_buf *buf,lpf_param *params)
{
	if(buf->output[0]==0&&
		 buf->output[1]==0&&
		 buf->output[2]==0&&
		 buf->input[0]==0&&
		 buf->input[1]==0&&
		 buf->input[2]==0)
	{
		buf->output[0]=curr_input;
		buf->output[1]=curr_input;
		buf->output[2]=curr_input;
		buf->input[0]=curr_input;
		buf->input[1]=curr_input;
		buf->input[2]=curr_input;
		return curr_input;
	}
	
  /* 加速度计Butterworth滤波 */
  /* 获取最新x(n) */
  buf->input[2]=curr_input;
  /* Butterworth滤波 */
  buf->output[2]=params->b[0] * buf->input[2]
													+params->b[1] * buf->input[1]
													+params->b[2] * buf->input[0]
													-params->a[1] * buf->output[1]
													-params->a[2] * buf->output[0];
  /* x(n) 序列保存 */
  buf->input[0]=buf->input[1];
  buf->input[1]=buf->input[2];
  /* y(n) 序列保存 */
  buf->output[0]=buf->output[1];
  buf->output[1]=buf->output[2];
	
	for(uint16_t i=0;i<3;i++)
	{
	  if(isnan(buf->output[i])==1
			||isnan(buf->input[i])==1)		
			{		
				buf->output[0]=curr_input;
				buf->output[1]=curr_input;
				buf->output[2]=curr_input;
				buf->input[0]=curr_input;
				buf->input[1]=curr_input;
				buf->input[2]=curr_input;
				return curr_input;
			}
	}	
  return buf->output[2];
}

/***************************************************
函数名: void set_cutoff_frequency(float sample_frequent, float cutoff_frequent,lpf_param *LPF)
说明:	二阶巴特沃斯低通滤波器参数设计
入口:	float sample_frequent-采样频率
			float cutoff_frequent-截止频率
			lpf_param *LPF-滤波器参数
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void set_cutoff_frequency(float sample_frequent, float cutoff_frequent,lpf_param *LPF)
{
  float fr = sample_frequent / cutoff_frequent;
  float ohm = tanf(M_PI_F / fr);
  float c = 1.0f + 2.0f * cosf(M_PI_F / 4.0f) * ohm + ohm * ohm;
  if (cutoff_frequent <= 0.0f) {
    // no filtering
    return;
  }
  LPF->b[0] = ohm * ohm / c;
  LPF->b[1] = 2.0f * LPF->b[0];
  LPF->b[2] = LPF->b[0];
  LPF->a[0]=1.0f;
  LPF->a[1] = 2.0f * (ohm * ohm - 1.0f) / c;
  LPF->a[2] = (1.0f - 2.0f * cosf(M_PI_F / 4.0f) * ohm + ohm * ohm) / c;
}

