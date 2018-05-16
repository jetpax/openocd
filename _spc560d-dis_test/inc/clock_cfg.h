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

#ifndef _CLOCK_CFG_H_
#define _CLOCK_CFG_H_

/*
 * Clock parameter settings.
 */
#define SPC5_XOSC_CLK                       16000000U
#define SPC5_OSC_BYPASS                     FALSE
#define SPC5_NO_INIT                        FALSE
#define SPC5_ALLOW_OVERCLOCK                FALSE
#define SPC5_DISABLE_WATCHDOG               TRUE
#define SPC5_FMPLL_IDF_VALUE                2U
#define SPC5_FMPLL_NDIV_VALUE               48U
#define SPC5_FMPLL_ODF                      SPC5_FMPLL_ODF_DIV8
#define SPC5_FMPLL_OPTIONS                  (0U)
#define SPC5_FMPLL_MR_INIT                  (0U)
#define SPC5_XOSCDIV_VALUE                  1U
#define SPC5_FIRCDIV_VALUE                  1U
#define SPC5_PERIPHERAL1_CLK_DIV_VALUE      2U
#define SPC5_PERIPHERAL2_CLK_DIV_VALUE      2U
#define SPC5_PERIPHERAL3_CLK_DIV_VALUE      2U
#define SPC5_SIRCDIV_VALUE                  4U
#define SPC5_ENABLE_API_RTC                 FALSE
#define SPC5_API_RTC_CLK_SRC                SPC5_RTCC_CLKSEL_SIRCDIV
#define SPC5_CLKOUT_SRC                     SPC5_CGM_OCDS_SELCTL_FMPLL
#define SPC5_CLKOUT_DIV_VALUE               2U
#define SPC5_ME_ME_BITS                     (0UL | SPC5_ME_ME_RESET | SPC5_ME_ME_SAFE | SPC5_ME_ME_DRUN | SPC5_ME_ME_RUN0 | SPC5_ME_ME_RUN1 | SPC5_ME_ME_RUN2 | SPC5_ME_ME_RUN3 | SPC5_ME_ME_HALT | SPC5_ME_ME_STOP | SPC5_ME_ME_STANDBY)
#define SPC5_ME_SAFE_MC_BITS                (0UL | SPC5_ME_MC_PDO | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_DRUN_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FMPLLON | SPC5_ME_MC_XOSCON | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FMPLL)
#define SPC5_ME_RUN0_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FMPLLON | SPC5_ME_MC_XOSCON | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FMPLL)
#define SPC5_ME_RUN1_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_RUN2_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_RUN3_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_HALT_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_NORMAL | SPC5_ME_MC_CFLAON_NORMAL | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_STOP_MC_BITS                (0UL | SPC5_ME_MC_MVRON | SPC5_ME_MC_DFLAON_PD | SPC5_ME_MC_CFLAON_PD | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_FIRC)
#define SPC5_ME_STANDBY_MC_BITS             (0UL | SPC5_ME_MC_PDO | SPC5_ME_MC_DFLAON_PD | SPC5_ME_MC_CFLAON_PD | SPC5_ME_MC_FIRCON | SPC5_ME_MC_SYSCLK_DISABLED)
#define SPC5_ME_RUN_PC3_BITS                (0UL)
#define SPC5_ME_RUN_PC4_BITS                (0UL)
#define SPC5_ME_RUN_PC5_BITS                (0UL)
#define SPC5_ME_RUN_PC6_BITS                (0UL)
#define SPC5_ME_RUN_PC7_BITS                (0UL)
#define SPC5_ME_LP_PC4_BITS                 (0UL)
#define SPC5_ME_LP_PC5_BITS                 (0UL)
#define SPC5_ME_LP_PC6_BITS                 (0UL)
#define SPC5_ME_LP_PC7_BITS                 (0UL)
#define SPC5_FINAL_RUNMODE                  SPC5_RUNMODE_RUN0
#define SPC5_CLOCK_FAILURE_HOOK()           irqSysHalt()

#endif /* _CLOCK_CFG_H_ */
