/*
    SPC5 RLA - Copyright (C) 2015 STMicroelectronics

    Licensed under the Apache License, Version 2.0 (the "License").
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    clock.h
 * @brief   SPC560Dxx clock subsystem header
 *
 * @addtogroup CLOCK
 * @{
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "platform.h"
#include "irq.h"
#include "clock_cfg.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Platform identification
 * @{
 */
#define PLATFORM_NAME               "SPC560Dxx Car Body and Convenience"
/** @} */

/**
 * @name    Common constants
 * @{
 */
#if !defined(FALSE) || defined(__DOXYGEN__)
#define FALSE                               0
#endif

#if !defined(TRUE) || defined(__DOXYGEN__)
#define TRUE                                1
#endif

#define CLOCK_SUCCESS               0U
#define CLOCK_FAILED                1U
/** @} */

/**
 * @name    Absolute Maximum Ratings
 * @{
 */
/**
 * @brief   Maximum XOSC clock frequency.
 */
#define SPC5_XOSC_CLK_MAX           16000000U

/**
 * @brief   Minimum XOSC clock frequency.
 */
#define SPC5_XOSC_CLK_MIN           4000000U

/**
 * @brief   Maximum FMPLLs input clock frequency.
 */
#define SPC5_FMPLLIN_MIN            4000000U

/**
 * @brief   Maximum FMPLLs input clock frequency.
 */
#define SPC5_FMPLLIN_MAX            48000000U

/**
 * @brief   Maximum FMPLLs VCO clock frequency.
 */
#define SPC5_FMPLLVCO_MAX           512000000U

/**
 * @brief   Maximum FMPLLs VCO clock frequency.
 */
#define SPC5_FMPLLVCO_MIN           256000000U

/**
 * @brief   Maximum FMPLL output clock frequency.
 */
#define SPC5_FMPLL_CLK_MAX          48000000U
/** @} */

/**
 * @name    Internal clock sources
 * @{
 */
#define SPC5_FIRC_CLK               16000000U   /**< Internal fast RC
                                                     oscillator.            */
#define SPC5_SIRC_CLK               128000U     /**< Internal RC slow
                                                     oscillator.            */
/** @} */

/**
 * @name    FMPLL_CR register bits definitions
 * @{
 */
#define SPC5_FMPLL_IDF_MASK         (15UL << 26)
#define SPC5_FMPLL_IDF(n)           (((uint32_t)((n) - 1U)) << 26)
#define SPC5_FMPLL_ODF_MASK         (3UL << 24)
#define SPC5_FMPLL_ODF_DIV2         (0UL << 24)
#define SPC5_FMPLL_ODF_DIV4         (1UL << 24)
#define SPC5_FMPLL_ODF_DIV8         (2UL << 24)
#define SPC5_FMPLL_ODF_DIV16        (3UL << 24)
#define SPC5_FMPLL_NDIV_MASK        (0x7FUL << 16)
#define SPC5_FMPLL_NDIV(n)          (((uint32_t)(n)) << 16)
#define SPC5_FMPLL_EN_PLL_SW        (1UL << 8)
#define SPC5_FMPLL_PLL_FAIL_MASK    (1UL << 2)
/** @} */

/**
 * @name    FMPLL_MR register bits definitions
 * @{
 */
#define SPC5_FMPLL_STRB_BYPASS      (1UL << 31)
#define SPC5_FMPLL_SPRD_SEL         (1UL << 29)
#define SPC5_FMPLL_MOD_PERIOD_MASK  (0x1FFFUL << 16)
#define SPC5_FMPLL_MOD_PERIOD(n)    (((uint32_t)(n)) << 16)
#define SPC5_FMPLL_FM_EN            (1UL << 15)
#define SPC5_FMPLL_INC_STEP_MASK    (0x7FFFUL << 0)
#define SPC5_FMPLL_INC_STEP(n)      (((uint32_t)(n)) << 0)
/** @} */

/**
 * @name    Clock selectors used in the various CGM SC registers
 * @{
 */
#define SPC5_CGM_SS_MASK            (15UL << 24)
#define SPC5_CGM_SS_FIRC            (0UL << 24)
#define SPC5_CGM_SS_FIRCDIV         (1UL << 24)
#define SPC5_CGM_SS_XOSC            (2UL << 24)
#define SPC5_CGM_SS_XOSCDIV         (3UL << 24)
#define SPC5_CGM_SS_FMPLL           (4UL << 24)
/** @} */

/**
 * @name    Clock selector value used in the RTTC register
 * @{
 */
#define SPC5_RTCC_CLKSEL_SIRCDIV    1U
#define SPC5_RTCC_CLKSEL_FIRCDIV    2U

/**
 * @name    Clock selectors used in the CGM_OCDS_SC register
 * @{
 */
#define SPC5_CGM_OCDS_SELCTL_MASK   (15UL << 24)
#define SPC5_CGM_OCDS_SELCTL_XOSC   (0UL << 24)
#define SPC5_CGM_OCDS_SELCTL_FIRC   (1UL << 24)
#define SPC5_CGM_OCDS_SELCTL_FMPLL  (2UL << 24)
#define SPC5_CGM_OCDS_SELCTL_SYSCLK (3UL << 24)
#define SPC5_CGM_OCDS_SELCTL_RTCCLK (4UL << 24)
/** @} */

/**
 * @name    Clock dividers used in the CGM_OCDS_SC register
 * @{
 */
#define SPC5_CGM_OCDS_SELDIV_MASK   (3UL << 28)
#define SPC5_CGM_OCDS_SELDIV_DIV1   (0UL << 28)
#define SPC5_CGM_OCDS_SELDIV_DIV2   (1UL << 28)
#define SPC5_CGM_OCDS_SELDIV_DIV4   (2UL << 28)
#define SPC5_CGM_OCDS_SELDIV_DIV8   (3UL << 28)
/** @} */

/**
 * @name    ME_GS register bits definitions
 * @{
 */
#define SPC5_ME_GS_SYSCLK_MASK      (15U << 0)
#define SPC5_ME_GS_SYSCLK_FIRC      (0U << 0)
#define SPC5_ME_GS_SYSCLK_FIRCDIV   (1U << 0)
#define SPC5_ME_GS_SYSCLK_XOSC      (2U << 0)
#define SPC5_ME_GS_SYSCLK_XOSCDIV   (3U << 0)
#define SPC5_ME_GS_SYSCLK_FMPLL     (4U << 0)
/** @} */

/**
 * @name    ME_ME register bits definitions
 * @{
 */
#define SPC5_ME_ME_RESET            (1UL << 0)
#define SPC5_ME_ME_TEST             (1UL << 1)
#define SPC5_ME_ME_SAFE             (1UL << 2)
#define SPC5_ME_ME_DRUN             (1UL << 3)
#define SPC5_ME_ME_RUN0             (1UL << 4)
#define SPC5_ME_ME_RUN1             (1UL << 5)
#define SPC5_ME_ME_RUN2             (1UL << 6)
#define SPC5_ME_ME_RUN3             (1UL << 7)
#define SPC5_ME_ME_HALT             (1UL << 8)
#define SPC5_ME_ME_STOP             (1UL << 10)
#define SPC5_ME_ME_STANDBY          (1UL << 13)
/** @} */

/**
 * @name    ME_xxx_MC registers bits definitions
 * @{
 */
#define SPC5_ME_MC_SYSCLK_MASK      (15UL << 0)
#define SPC5_ME_MC_SYSCLK(n)        (((uint32_t)(n)) << 0)
#define SPC5_ME_MC_SYSCLK_FIRC      SPC5_ME_MC_SYSCLK(0)
#define SPC5_ME_MC_SYSCLK_FIRCDIV   SPC5_ME_MC_SYSCLK(1)
#define SPC5_ME_MC_SYSCLK_XOSC      SPC5_ME_MC_SYSCLK(2)
#define SPC5_ME_MC_SYSCLK_XOSCDIV   SPC5_ME_MC_SYSCLK(3)
#define SPC5_ME_MC_SYSCLK_FMPLL     SPC5_ME_MC_SYSCLK(4)
#define SPC5_ME_MC_SYSCLK_DISABLED  SPC5_ME_MC_SYSCLK(15)
#define SPC5_ME_MC_FIRCON           (1UL << 4)
#define SPC5_ME_MC_XOSCON           (1UL << 5)
#define SPC5_ME_MC_FMPLLON          (1UL << 6)
#define SPC5_ME_MC_CFLAON_MASK      (3UL << 16)
#define SPC5_ME_MC_CFLAON(n)        (((uint32_t)(n)) << 16)
#define SPC5_ME_MC_CFLAON_PD        (1UL << 16)
#define SPC5_ME_MC_CFLAON_LP        (2UL << 16)
#define SPC5_ME_MC_CFLAON_NORMAL    (3UL << 16)
#define SPC5_ME_MC_DFLAON_MASK      (3UL << 18)
#define SPC5_ME_MC_DFLAON(n)        (((uint32_t)(n)) << 18)
#define SPC5_ME_MC_DFLAON_PD        (1UL << 18)
#define SPC5_ME_MC_DFLAON_LP        (2UL << 18)
#define SPC5_ME_MC_DFLAON_NORMAL    (3UL << 18)
#define SPC5_ME_MC_MVRON            (1UL << 20)
#define SPC5_ME_MC_PDO              (1UL << 23)
/** @} */

/**
 * @name    ME_MCTL register bits definitions
 * @{
 */
#define SPC5_ME_MCTL_KEY            0x5AF0UL
#define SPC5_ME_MCTL_KEY_INV        0xA50FUL
#define SPC5_ME_MCTL_MODE_MASK      (15UL << 28)
#define SPC5_ME_MCTL_MODE(n)        (((uint32_t)(n)) << 28)
/** @} */

/**
 * @name    ME_RUN_PCx registers bits definitions
 * @{
 */
#define SPC5_ME_RUN_PC_TEST         (1UL << 1)
#define SPC5_ME_RUN_PC_SAFE         (1UL << 2)
#define SPC5_ME_RUN_PC_DRUN         (1UL << 3)
#define SPC5_ME_RUN_PC_RUN0         (1UL << 4)
#define SPC5_ME_RUN_PC_RUN1         (1UL << 5)
#define SPC5_ME_RUN_PC_RUN2         (1UL << 6)
#define SPC5_ME_RUN_PC_RUN3         (1UL << 7)
/** @} */

/**
 * @name    ME_LP_PCx registers bits definitions
 * @{
 */
#define SPC5_ME_LP_PC_HALT          (1UL << 8)
#define SPC5_ME_LP_PC_STOP          (1UL << 10)
#define SPC5_ME_LP_PC_STANDBY       (1UL << 13)
/** @} */

/**
 * @name    ME_PCTL registers bits definitions
 * @{
 */
#define SPC5_ME_PCTL_RUN_MASK       (7UL << 0)
#define SPC5_ME_PCTL_RUN(n)         (((uint8_t)(n)) << 0)
#define SPC5_ME_PCTL_LP_MASK        (7UL << 3)
#define SPC5_ME_PCTL_LP(n)          (((uint8_t)(n)) << 3)
#define SPC5_ME_PCTL_DBG            (1UL << 6)
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Disables the clocks initialization.
 */
#if !defined(SPC5_NO_INIT) || defined(__DOXYGEN__)
#define SPC5_NO_INIT                        FALSE
#endif

/**
 * @brief   Disables the overclock checks.
 */
#if !defined(SPC5_ALLOW_OVERCLOCK) || defined(__DOXYGEN__)
#define SPC5_ALLOW_OVERCLOCK                FALSE
#endif

/**
 * @brief   Disables the watchdog on start.
 */
#if !defined(SPC5_DISABLE_WATCHDOG) || defined(__DOXYGEN__)
#define SPC5_DISABLE_WATCHDOG               TRUE
#endif

/**
 * @brief   Bypasses oscillator cirtuit.
 */
#if !defined(SPC5_OSC_BYPASS) || defined(__DOXYGEN__)
#define SPC5_OSC_BYPASS                     FALSE
#endif

/**
 * @brief   FMPLL IDF divider value.
 * @note    The default value is calculated for XOSC=8MHz and PHI=48MHz.
 */
#if !defined(SPC5_FMPLL_IDF_VALUE) || defined(__DOXYGEN__)
#define SPC5_FMPLL_IDF_VALUE                1U
#endif

/**
 * @brief   FMPLL NDIV divider value.
 * @note    The default value is calculated for XOSC=8MHz and PHI=48MHz.
 */
#if !defined(SPC5_FMPLL_NDIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_FMPLL_NDIV_VALUE               48U
#endif

/**
 * @brief   FMPLL ODF divider value.
 * @note    The default value is calculated for XOSC=8MHz and PHI=48MHz.
 */
#if !defined(SPC5_FMPLL_ODF) || defined(__DOXYGEN__)
#define SPC5_FMPLL_ODF                      SPC5_FMPLL_ODF_DIV8
#endif

/**
 * @brief   FMPLL CR register extra options.
 */
#if !defined(SPC5_FMPLL_OPTIONS) || defined(__DOXYGEN__)
#define SPC5_FMPLL_OPTIONS                  0U
#endif

/**
 * @brief   FMPLL MR register initialization.
 */
#if !defined(SPC5_FMPLL_MR_INIT) || defined(__DOXYGEN__)
#define SPC5_FMPLL_MR_INIT                  0U
#endif

/**
 * @brief   XOSC divider value.
 * @note    The allowed range is 1...32.
 */
#if !defined(SPC5_XOSCDIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_XOSCDIV_VALUE                  1U
#endif

/**
 * @brief   Fast IRC divider value.
 * @note    The allowed range is 1...32.
 */
#if !defined(SPC5_FIRCDIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_FIRCDIV_VALUE                  1U
#endif

/**
 * @brief   Peripherals Set 1 clock divider value.
 * @note    Zero means disabled clock.
 */
#if !defined(SPC5_PERIPHERAL1_CLK_DIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_PERIPHERAL1_CLK_DIV_VALUE      2U
#endif

/**
 * @brief   Peripherals Set 2 clock divider value.
 * @note    Zero means disabled clock.
 */
#if !defined(SPC5_PERIPHERAL2_CLK_DIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_PERIPHERAL2_CLK_DIV_VALUE      2U
#endif

/**
 * @brief   Peripherals Set 3 clock divider value.
 * @note    Zero means disabled clock.
 */
#if !defined(SPC5_PERIPHERAL3_CLK_DIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_PERIPHERAL3_CLK_DIV_VALUE      2U
#endif

/**
 * @brief   SIRC clock divider value.
 */
#if !defined(SPC5_SIRCDIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_SIRCDIV_VALUE                  4U
#endif

/**
 * @brief   API/RTC enable.
 */
#if !defined(SPC5_ENABLE_API_RTC) || defined(__DOXYGEN__)
#define SPC5_ENABLE_API_RTC                 FALSE
#endif

/**
 * @brief   API/RTC clock source.
 */
#if !defined(SPC5_API_RTC_CLK_SRC) || defined(__DOXYGEN__)
#define SPC5_API_RTC_CLK_SRC                SPC5_RTCC_CLKSEL_SIRCDIV
#endif

/**
 * @brief   Clock Out clock source.
 */
#if !defined(SPC5_CLKOUT_SRC) || defined(__DOXYGEN__)
#define SPC5_CLKOUT_SRC                     SPC5_CGM_OCDS_SELCTL_XOSC
#endif

/**
 * @brief   Clock Out clock divider value.
 * @note    Possible values are 1, 2, 4 and 8.
 */
#if !defined(SPC5_CLKOUT_DIV_VALUE) || defined(__DOXYGEN__)
#define SPC5_CLKOUT_DIV_VALUE               1U
#endif

/**
 * @brief   Active run modes in ME_ME register.
 * @note    Modes RESET, SAFE, DRUN, and RUN0 modes are always enabled, there
 *          is no need to specify them.
 */
#if !defined(SPC5_ME_ME_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_ME_BITS                     (SPC5_ME_ME_RUN1 |              \
                                             SPC5_ME_ME_RUN2 |              \
                                             SPC5_ME_ME_RUN3 |              \
                                             SPC5_ME_ME_HALT |              \
                                             SPC5_ME_ME_STOP |              \
                                             SPC5_ME_ME_STANDBY)
#endif

/**
 * @brief   TEST mode settings.
 */
#if !defined(SPC5_ME_TEST_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_TEST_MC_BITS                (SPC5_ME_MC_SYSCLK_FIRC |       \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   SAFE mode settings.
 */
#if !defined(SPC5_ME_SAFE_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_SAFE_MC_BITS                (SPC5_ME_MC_SYSCLK_FIRC |       \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON |             \
                                             SPC5_ME_MC_PDO)
#endif

/**
 * @brief   DRUN mode settings.
 */
#if !defined(SPC5_ME_DRUN_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_DRUN_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   RUN0 mode settings.
 */
#if !defined(SPC5_ME_RUN0_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN0_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   RUN1 mode settings.
 */
#if !defined(SPC5_ME_RUN1_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN1_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   RUN2 mode settings.
 */
#if !defined(SPC5_ME_RUN2_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN2_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   RUN3 mode settings.
 */
#if !defined(SPC5_ME_RUN3_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN3_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   HALT mode settings.
 */
#if !defined(SPC5_ME_HALT_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_HALT_MC_BITS                (SPC5_ME_MC_SYSCLK_FMPLL |      \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_XOSCON |            \
                                             SPC5_ME_MC_FMPLLON |           \
                                             SPC5_ME_MC_CFLAON_NORMAL |     \
                                             SPC5_ME_MC_DFLAON_NORMAL |     \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   STOP mode settings.
 */
#if !defined(SPC5_ME_STOP_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_STOP_MC_BITS                (SPC5_ME_MC_SYSCLK_FIRC |       \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_CFLAON_PD |         \
                                             SPC5_ME_MC_DFLAON_PD |         \
                                             SPC5_ME_MC_MVRON)
#endif

/**
 * @brief   STANDBY mode settings.
 */
#if !defined(SPC5_ME_STANDBY_MC_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_STANDBY_MC_BITS             (SPC5_ME_MC_SYSCLK_DISABLED |   \
                                             SPC5_ME_MC_FIRCON |            \
                                             SPC5_ME_MC_CFLAON_PD |         \
                                             SPC5_ME_MC_DFLAON_PD |         \
                                             SPC5_ME_MC_PDO)
#endif

/**
 * @brief   Peripheral mode 0 (run mode).
 * @note    Do not change this setting, it is expected to be the "never run"
 *          mode.
 */
#if !defined(SPC5_ME_RUN_PC0_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC0_BITS                0
#endif

/**
 * @brief   Peripheral mode 1 (run mode).
 * @note    Do not change this setting, it is expected to be the "always run"
 *          mode.
 */
#if !defined(SPC5_ME_RUN_PC1_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC1_BITS                (SPC5_ME_RUN_PC_TEST |          \
                                             SPC5_ME_RUN_PC_SAFE |          \
                                             SPC5_ME_RUN_PC_DRUN |          \
                                             SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 2 (run mode).
 * @note    Do not change this setting, it is expected to be the "only during
 *          normal run" mode.
 */
#if !defined(SPC5_ME_RUN_PC2_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC2_BITS                (SPC5_ME_RUN_PC_DRUN |          \
                                             SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 3 (run mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_RUN_PC3_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC3_BITS                (SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 4 (run mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_RUN_PC4_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC4_BITS                (SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 5 (run mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_RUN_PC5_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC5_BITS                (SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 6 (run mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_RUN_PC6_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC6_BITS                (SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 7 (run mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_RUN_PC7_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_RUN_PC7_BITS                (SPC5_ME_RUN_PC_RUN0 |          \
                                             SPC5_ME_RUN_PC_RUN1 |          \
                                             SPC5_ME_RUN_PC_RUN2 |          \
                                             SPC5_ME_RUN_PC_RUN3)
#endif

/**
 * @brief   Peripheral mode 0 (low power mode).
 * @note    Do not change this setting, it is expected to be the "never run"
 *          mode.
 */
#if !defined(SPC5_ME_LP_PC0_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC0_BITS                 0
#endif

/**
 * @brief   Peripheral mode 1 (low power mode).
 * @note    Do not change this setting, it is expected to be the "always run"
 *          mode.
 */
#if !defined(SPC5_ME_LP_PC1_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC1_BITS                 (SPC5_ME_LP_PC_HALT |           \
                                             SPC5_ME_LP_PC_STOP |           \
                                             SPC5_ME_LP_PC_STANDBY)
#endif

/**
 * @brief   Peripheral mode 2 (low power mode).
 * @note    Do not change this setting, it is expected to be the "halt only"
 *          mode.
 */
#if !defined(SPC5_ME_LP_PC2_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC2_BITS                 (SPC5_ME_LP_PC_HALT)
#endif

/**
 * @brief   Peripheral mode 3 (low power mode).
 * @note    Do not change this setting, it is expected to be the "stop only"
 *          mode.
 */
#if !defined(SPC5_ME_LP_PC3_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC3_BITS                 (SPC5_ME_LP_PC_STOP)
#endif

/**
 * @brief   Peripheral mode 4 (low power mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_LP_PC4_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC4_BITS                 (SPC5_ME_LP_PC_HALT |           \
                                             SPC5_ME_LP_PC_STOP)
#endif

/**
 * @brief   Peripheral mode 5 (low power mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_LP_PC5_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC5_BITS                 (SPC5_ME_LP_PC_HALT |           \
                                             SPC5_ME_LP_PC_STOP)
#endif

/**
 * @brief   Peripheral mode 6 (low power mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_LP_PC6_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC6_BITS                 (SPC5_ME_LP_PC_HALT |           \
                                             SPC5_ME_LP_PC_STOP)
#endif

/**
 * @brief   Peripheral mode 7 (low power mode).
 * @note    Not defined, available to application-specific modes.
 */
#if !defined(SPC5_ME_LP_PC7_BITS) || defined(__DOXYGEN__)
#define SPC5_ME_LP_PC7_BITS                 (SPC5_ME_LP_PC_HALT |           \
                                             SPC5_ME_LP_PC_STOP)
#endif

/**
 * @brief   Final run mode after initialization.
 * @note    It can be selected between DRUN, RUN0...RUN3.
 */
#if !defined(SPC5_FINAL_RUNMODE) || defined(__DOXYGEN__)
#define SPC5_FINAL_RUNMODE                  SPC5_RUNMODE_RUN0
#endif

/**
 * @brief   Clock initialization failure hook.
 * @note    The default is to stop the system and let the RTC restart it.
 * @note    The hook code must not return.
 */
#if !defined(SPC5_CLOCK_FAILURE_HOOK) || defined(__DOXYGEN__)
#define SPC5_CLOCK_FAILURE_HOOK()           irqSysHalt()
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/* Check on the XOSC frequency.*/
#if (SPC5_XOSC_CLK < SPC5_XOSC_CLK_MIN) ||                                  \
    (SPC5_XOSC_CLK > SPC5_XOSC_CLK_MAX)
#error "invalid SPC5_XOSC_CLK value specified"
#endif

/* Check on the XOSC divider.*/
#if (SPC5_XOSCDIV_VALUE < 1U) || (SPC5_XOSCDIV_VALUE > 32U)
#error "invalid SPC5_XOSCDIV_VALUE value specified"
#endif

/* Check on the FIRC divider.*/
#if (SPC5_FIRCDIV_VALUE < 1U) || (SPC5_FIRCDIV_VALUE > 32U)
#error "invalid SPC5_FIRCDIV_VALUE value specified"
#endif

/* Check on SPC5_FMPLL_IDF_VALUE.*/
#if (SPC5_FMPLL_IDF_VALUE < 1U) || (SPC5_FMPLL_IDF_VALUE > 15U)
#error "invalid SPC5_FMPLL_IDF_VALUE value specified"
#endif

/* Check on SPC5_FMPLL_NDIV_VALUE.*/
#if (SPC5_FMPLL_NDIV_VALUE < 32U) || (SPC5_FMPLL_NDIV_VALUE > 96U)
#error "invalid SPC5_FMPLL_NDIV_VALUE value specified"
#endif

/* Check on SPC5_FMPLL_ODF.*/
#if (SPC5_FMPLL_ODF == SPC5_FMPLL_ODF_DIV2)
#define SPC5_FMPLL_ODF_VALUE    2U
#elif (SPC5_FMPLL_ODF == SPC5_FMPLL_ODF_DIV4)
#define SPC5_FMPLL_ODF_VALUE    4U
#elif (SPC5_FMPLL_ODF == SPC5_FMPLL_ODF_DIV8)
#define SPC5_FMPLL_ODF_VALUE    8U
#elif (SPC5_FMPLL_ODF == SPC5_FMPLL_ODF_DIV16)
#define SPC5_FMPLL_ODF_VALUE    16U
#else
#error "invalid SPC5_FMPLL_ODF value specified"
#endif

/**
 * @brief   SPC5_FMPLL_VCO_CLK clock point.
 */
#define SPC5_FMPLL_VCO_CLK                                                  \
  ((SPC5_XOSC_CLK / SPC5_FMPLL_IDF_VALUE) * SPC5_FMPLL_NDIV_VALUE)

/* Check on FMPLL VCO output.*/
#if (SPC5_FMPLL_VCO_CLK < SPC5_FMPLLVCO_MIN) ||                             \
    (SPC5_FMPLL_VCO_CLK > SPC5_FMPLLVCO_MAX)
#error "SPC5_FMPLL_VCO_CLK outside acceptable range (SPC5_FMPLLVCO_MIN...SPC5_FMPLLVCO_MAX)"
#endif

/**
 * @brief   SPC5_FMPLL_CLK clock point.
 */
#define SPC5_FMPLL_CLK                                                      \
  (SPC5_FMPLL_VCO_CLK / SPC5_FMPLL_ODF_VALUE)

/* Check on SPC5_FMPLL_CLK.*/
#if (SPC5_FMPLL_CLK > SPC5_FMPLL_CLK_MAX) && (SPC5_ALLOW_OVERCLOCK == FALSE)
#error "SPC5_FMPLL_CLK outside acceptable range (0...SPC5_FMPLL_CLK_MAX)"
#endif

/* Check on the peripherals set 1 clock divider settings.*/
#if SPC5_PERIPHERAL1_CLK_DIV_VALUE == 0U
#define SPC5_CGM_SC_DC0         0U
#elif (SPC5_PERIPHERAL1_CLK_DIV_VALUE >= 1U) &&                              \
      (SPC5_PERIPHERAL1_CLK_DIV_VALUE <= 16U)
#define SPC5_CGM_SC_DC0         (uint8_t)(0x80U | (uint8_t)(SPC5_PERIPHERAL1_CLK_DIV_VALUE - 1U))
#else
#error "invalid SPC5_PERIPHERAL1_CLK_DIV_VALUE value specified"
#endif

/* Check on the peripherals set 2 clock divider settings.*/
#if SPC5_PERIPHERAL2_CLK_DIV_VALUE == 0U
#define SPC5_CGM_SC_DC1         0U
#elif (SPC5_PERIPHERAL2_CLK_DIV_VALUE >= 1U) &&                              \
      (SPC5_PERIPHERAL2_CLK_DIV_VALUE <= 16U)
#define SPC5_CGM_SC_DC1         (uint8_t)(0x80U | (uint8_t)(SPC5_PERIPHERAL2_CLK_DIV_VALUE - 1U))
#else
#error "invalid SPC5_PERIPHERAL2_CLK_DIV_VALUE value specified"
#endif

/* Check on the peripherals set 3 clock divider settings.*/
#if SPC5_PERIPHERAL3_CLK_DIV_VALUE == 0U
#define SPC5_CGM_SC_DC2         0U
#elif (SPC5_PERIPHERAL3_CLK_DIV_VALUE >= 1U) &&                              \
      (SPC5_PERIPHERAL3_CLK_DIV_VALUE <= 16U)
#define SPC5_CGM_SC_DC2         (uint8_t)(0x80U | (uint8_t)(SPC5_PERIPHERAL3_CLK_DIV_VALUE - 1U))
#else
#error "invalid SPC5_PERIPHERAL3_CLK_DIV_VALUE value specified"
#endif

/* Check on the SIRCDIV divider.*/
#if (SPC5_SIRCDIV_VALUE < 1U) || (SPC5_SIRCDIV_VALUE > 32U)
#error "invalid SPC5_SIRCDIV_VALUE value specified"
#endif

/* Check on the CLKOUT divider settings.*/
#if (SPC5_CLKOUT_DIV_VALUE == 1U)
#define SPC5_CGM_OCDS_SELDIV    SPC5_CGM_OCDS_SELDIV_DIV1
#elif (SPC5_CLKOUT_DIV_VALUE == 2U)
#define SPC5_CGM_OCDS_SELDIV    SPC5_CGM_OCDS_SELDIV_DIV2
#elif (SPC5_CLKOUT_DIV_VALUE == 4U)
#define SPC5_CGM_OCDS_SELDIV    SPC5_CGM_OCDS_SELDIV_DIV4
#elif (SPC5_CLKOUT_DIV_VALUE == 8U)
#define SPC5_CGM_OCDS_SELDIV    SPC5_CGM_OCDS_SELDIV_DIV8
#else
#error "invalid SPC5_CLKOUT_DIV_VALUE value specified"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Run modes.
 */
typedef enum {
  SPC5_RUNMODE_TEST    = 1,
  SPC5_RUNMODE_SAFE    = 2,
  SPC5_RUNMODE_DRUN    = 3,
  SPC5_RUNMODE_RUN0    = 4,
  SPC5_RUNMODE_RUN1    = 5,
  SPC5_RUNMODE_RUN2    = 6,
  SPC5_RUNMODE_RUN3    = 7,
  SPC5_RUNMODE_HALT    = 8,
  SPC5_RUNMODE_STOP    = 10,
  SPC5_RUNMODE_STANDBY = 13
} spc5_runmode_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void clockInit(void);
  void spc_clock_init(void);
  void __early_init(void);
  uint8_t SPCSetRunMode(spc5_runmode_t mode);
  void SPCSetPeripheralClockMode(uint32_t n, uint32_t pctl);
#if (SPC5_NO_INIT == FALSE)
  uint32_t SPCGetSystemClock(void);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _CLOCK_H_ */

/** @} */
