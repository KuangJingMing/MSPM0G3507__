#include "alert.h"
#include "freertos.h"
#include "task.h"

static Color current_color = COLOR_GREEN;
static uint8_t alert_count = 0;
static uint8_t alert_enable = 0;
static uint16_t alert_time = 300;
static TickType_t last_tick_time = 0;
static uint8_t current_state = 0;
static uint8_t completed_cycles = 0;

void play_alert_blocking(uint8_t count, Color c) {
	for (int i = 0; i < count; i++) {
		led_set_color(c);
		beep_on();
		delay_ms(200);
		beep_off();
		led_set_color(COLOR_OFF);
		delay_ms(200);
	}
}

void set_alert_color(Color c) {
	current_color = c;
}

void set_alert_count(uint8_t count) {
	alert_count = count * 2;
}

void set_alert_interval_time(uint16_t time) {
	alert_time = time;
}

// 需要修改 start_alert 和 stop_alert 来初始化 completed_cycles
void start_alert(void) {
	// 只有在警报未启用且设置了次数时才启动
	if (!alert_enable && (alert_count / 2) > 0) { // 确保有至少一个完整的循环
        alert_enable = 1;
        completed_cycles = 0; // 重置已完成的循环次数
        current_state = 0; // 初始状态为关
        last_tick_time = xTaskGetTickCount(); // 初始化计时器，以便立即开始第一个间隔
        // 立即执行第一次状态更新（如果需要，否则等待第一个间隔）
        // alert_ticks(); // 如果希望 start_alert 立即触发第一次状态更新，可以在这里调用
    }
}
void stop_alert(void) {
    alert_enable = 0;
    completed_cycles = 0; // 清零已完成循环次数
    // 确保最后是关闭状态
    led_set_color(COLOR_OFF);
    beep_off();
    current_state = 0; // 重置状态
}


void alert_ticks(void) {
	if (!alert_enable) return;

	TickType_t current_tick_time = xTaskGetTickCount();
	// 检查时间间隔是否到达
	if (current_tick_time - last_tick_time >= pdMS_TO_TICKS(alert_time)) {
		// 时间间隔到了，切换状态
		last_tick_time = current_tick_time; // 更新上次时间
		if (current_state == 0) { // 当前是关状态，切换到开
			led_set_color(current_color);
			beep_on();
			current_state = 1;
		} else { // 当前是开状态，切换到关
			led_set_color(COLOR_OFF);
			beep_off();
			current_state = 0;
      completed_cycles++;
		}
		// 检查是否完成所有预期的循环次数 (原始的 count)
		// alert_count 在 set_alert_count 中设置的是 count * 2，
		// 我们可以用它来判断是否完成了 count 个循环
		if (completed_cycles >= alert_count / 2) { // 或者直接用原始的 count 变量
				alert_enable = 0; // 停止提示
				// 确保最后是关闭状态
				led_set_color(COLOR_OFF);
				beep_off();
				current_state = 0; // 重置状态
				completed_cycles = 0; // 重置计数
		}
	}
}