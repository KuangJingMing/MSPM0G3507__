#include "common.h"

//#include "log_config.h"
#include "log.h"


int main(void)
{
    SYSCFG_DL_init();
		Gray_Detection_Init();
		OLED_Init();
		//OLED_Test();
	  Motor_Init();//电机初始化
		Encoder_init();//编码器初始化
	  Motor_Control(1200, 1200);
    for (;;)
    {
		  float right=get_right_motor_speed();
	    float left=get_left_motor_speed();
			//Gray_ReadData();
			//delay_ms(1000);
			delay_ms(20);
			OLED_Showdecimal(0,0,right,4,2,16,1);
	    OLED_Showdecimal(1,3,left,4,2,16,1);
	    OLED_ShowNum(2,6,left_motor_period_cnt,4,16,1);
	    OLED_ShowNum(60,6,right_motor_period_cnt,4,16,1);
    }
    return 0;
}
