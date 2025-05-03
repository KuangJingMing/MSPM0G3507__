// common.h
#ifndef COMMON_INCLUDE_H
#define COMMON_INCLUDE_H

#include "FreeRTOSConfig.h"

#include <stdio.h>
#include "ti_msp_dl_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oled_driver.h"
#include "gray_detection_app.h"
#include "motor_hardware.h"
#include "encoder.h"
#include "timers.h"
#include "motor_app.h"
#include "embedfire_protocol.h"
#include "serialplot_protocol.h"
#include "button_app.h"
#include "menu_app.h"
#include "encoder_app.h"
#include "icm20608.h"
#include "imu_app.h"
#include "driver_at24cxx.h" // LibDriver AT24CXX
#include "tests.h"
#include "eeprom_parameter.h"
#include "beep.h"
#include "rgb_led.h"
#include "alert.h"
#include "timer_task_notification.h"
#include "periodic_event_task.h"
#include "menu_logic.h"

#endif // COMMON_H