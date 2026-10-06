/**
 * @file    sensor.h
 * @brief   Sensor Device Layer interface for Smart Room Control system
 * @note    Architecture Layer: DEVICE
 *          - Provides interface for environmental and motion telemetry acquisition.
 *          - Implemented by Coder 2 (DHT11/DHT22, LDR/BH1750, PIR, etc.).
 *          - All getter functions return standardized engineering units.
 */

#ifndef SENSOR_H
#define SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief  Khởi tạo phần cứng và ngoại vi cho toàn bộ cảm biến.
 *         Coder 2 cấu hình các GPIO/ADC/Timer/Bus cần thiết ở đây.
 */
void sensor_init(void);

/**
 * @brief  Hàm cập nhật hoặc đọc mẫu mới từ các cảm biến.
 *         Được gọi định kỳ trong executive loop (app_loop).
 */
void sensor_update(void);

/**
 * @brief  Lấy giá trị nhiệt độ phòng hiện tại.
 * @return Nhiệt độ theo độ C (float, ví dụ: 28.5f)
 */
float sensor_get_temperature(void);

/**
 * @brief  Lấy giá trị độ ẩm không khí hiện tại.
 * @return Độ ẩm tương đối theo % (float, dải: 0.0f - 100.0f, ví dụ: 65.0f)
 */
float sensor_get_humidity(void);

/**
 * @brief  Lấy giá trị độ sáng môi trường hiện tại.
 * @return Độ sáng theo % (float, dải: 0.0f - 100.0f, ví dụ: 85.5f)
 */
float sensor_get_light(void);

/**
 * @brief  Lấy trạng thái cảm biến hồng ngoại phát hiện người PIR.
 * @return bool (true: phát hiện có người / motion detected, false: không có người)
 */
bool sensor_get_pir(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H */
