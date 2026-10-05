/**
 * @file    app.h
 * @brief   Application layer for Smart Room Control system
 * @note    Contains top-level business and operational logic.
 *          No direct HAL hardware calls allowed.
 */

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes application states, services, and renders initial UI.
 */
void app_init(void);

/**
 * @brief  Main application executive loop step.
 *         Should be called continuously from main's while(1).
 */
void app_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
