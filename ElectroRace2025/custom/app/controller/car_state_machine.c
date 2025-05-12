#include "car_state_machine.h"

// 全局动作队列
static action_queue_t action_queue = {
    .head = 0,
    .tail = 0,
    .size = 0
};

// 动作配置表，用户可直接修改此表定义动作序列和循环模式
static action_config_t action_config = {
    .actions = {
        {ACTION_GO_STRAIGHT, 155.0f},   // 直行155cm
        {ACTION_SPIN_TURN, 90.0f},      // 旋转到90度
        {ACTION_GO_STRAIGHT, 125.0f},   // 直行125cm
        {ACTION_SPIN_TURN, 180.0f},     // 旋转到180度
        {ACTION_GO_STRAIGHT, 150.0f},   // 直行150cm
        {ACTION_SPIN_TURN, -90.0f},     // 旋转到-90度
        {ACTION_GO_STRAIGHT, 130.0f},   // 直行135cm
				{ACTION_SPIN_TURN, 0},     			// 旋转到0度
    },
    .action_count = 8,                  // 动作数量，需与实际动作条目数一致
    .is_loop_enabled = true             // 默认不循环，用户可修改为 true 启用循环
};


/**
 * @brief 初始化动作队列
 */
void action_queue_init(void) {
    action_queue.head = 0;
    action_queue.tail = 0;
    action_queue.size = 0;
}
/**
 * @brief 向队列中添加一个动作
 * @param type 动作类型
 * @param target_value 目标值（里程或角度）
 * @return true 表示添加成功，false 表示队列已满
 */
bool action_queue_enqueue(action_type_t type, float target_value) {
    if (action_queue.size >= QUEUE_MAX_SIZE) {
        return false;  // 队列已满
    }
    action_queue.actions[action_queue.tail].type = type;
    action_queue.actions[action_queue.tail].target_value = target_value;
    action_queue.tail = (action_queue.tail + 1) % QUEUE_MAX_SIZE;
    action_queue.size++;
    return true;
}
/**
 * @brief 从队列中取出一个动作
 * @param action 存储取出的动作
 * @return true 表示取出成功，false 表示队列为空
 */
bool action_queue_dequeue(car_action_t *action) {
    if (action_queue.size == 0) {
        return false;  // 队列为空
    }
    *action = action_queue.actions[action_queue.head];
    action_queue.head = (action_queue.head + 1) % QUEUE_MAX_SIZE;
    action_queue.size--;
    return true;
}
/**
 * @brief 判断队列是否为空
 * @return true 表示队列为空，false 表示队列非空
 */
bool action_queue_is_empty(void) {
    return (action_queue.size == 0);
}
/**
 * @brief 从配置表中加载动作到队列
 * @return true 表示加载成功，false 表示队列容量不足
 */
bool action_queue_load_from_config(void) {
    action_queue_init();  // 清空当前队列
    for (uint8_t i = 0; i < action_config.action_count; i++) {
        if (!action_queue_enqueue(action_config.actions[i].type, action_config.actions[i].target_value)) {
            return false;  // 队列容量不足
        }
    }
    return true;
}
/**
 * @brief 初始化小车路径，从配置表加载动作到队列
 */
void car_path_init(void) {
    action_queue_load_from_config();  // 初始时从配置表加载动作
}

/**
 * @brief 小车状态机，使用队列管理动作并支持循环或单次执行
 */
void car_state_machine(void) {
    static car_action_t current_action;  // 当前正在执行的动作
    static bool is_action_active = false;  // 是否有动作正在执行

    // 如果当前没有动作在执行，且队列为空，且循环模式启用，则重新加载配置表
    if (!is_action_active && action_queue_is_empty() && action_config.is_loop_enabled) {
        action_queue_load_from_config();  // 重新加载动作，实现循环
    }

    // 如果当前没有动作在执行，且队列不为空，则从队列中取出一个动作
    if (!is_action_active && !action_queue_is_empty()) {
        if (action_queue_dequeue(&current_action)) {
            is_action_active = true;
        }
    }

    // 如果有动作在执行，调用对应的控制函数
    if (is_action_active) {
        bool action_completed = false;
        if (current_action.type == ACTION_GO_STRAIGHT) {
            action_completed = car_go_cm(current_action.target_value);
        } else if (current_action.type == ACTION_SPIN_TURN) {
            action_completed = spin_turn(current_action.target_value);
        }

        // 如果当前动作完成，标记为未激活，准备处理下一个动作
        if (action_completed) {
            is_action_active = false;
        }
    }
}
