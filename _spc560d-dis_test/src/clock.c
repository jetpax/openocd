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
 * @file    clock.c
 * @brief   SPC560Dxx clock subsystem source.
 *
 * @addtogroup CLOCK
 * @{
 */

#include "clock.h"

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Clock initialization.
 *
 * @notapi
 */
void clockInit(void) {

  /* The system is switched in the final run mode (default is RUN0 mode).*/
  if (SPCSetRunMode(SPC5_FINAL_RUNMODE) == CLOCK_FAILED) {
    SPC5_CLOCK_FAILURE_HOOK();
  }
}

/**
 * @brief   SPC560Bxx clocks and PLL initialization.
 * @note    This function must be invoked only after the system reset.
 *
 * @special
 */
void spc_clock_init(void) {

  /* Waiting for IRC stabilization before attempting anything else.*/
  while (ME.GS.B.S_FIRC == 0U){
  }

#if (SPC5_NO_INIT == FALSE)

#if (SPC5_DISABLE_WATCHDOG == TRUE)
  /* SWT disabled.*/
  SWT.SR.R = 0xC520;
  SWT.SR.R = 0xD928;
  SWT.CR.R = 0xFF00000AU;
#endif

  /* SSCM initialization. Setting up the most restrictive handling of
     invalid accesses to peripherals.*/
  SSCM.ERROR.R = 3;                             /* PAE and RAE bits.        */

  /* RGM errors clearing.*/
  RGM.FES.R         = 0xFFFFU;
  RGM.DES.R         = 0xFFFFU;

  /* Oscillators dividers setup.*/
  CGM.FIRC_CTL.B.RCDIV   = (uint8_t)(SPC5_FIRCDIV_VALUE - 1U);
  CGM.FXOSC_CTL.B.OSCDIV = (uint8_t)(SPC5_XOSCDIV_VALUE - 1U);
  CGM.SIRC_CTL.B.SIRCDIV = (uint8_t)(SPC5_SIRCDIV_VALUE - 1U);

  /* The system must be in DRUN mode on entry, if this is not the case then
     it is considered a serious anomaly.*/
  if (ME.GS.B.S_CURRENTMODE != (uint32_t)SPC5_RUNMODE_DRUN) {
    SPC5_CLOCK_FAILURE_HOOK();
  }

#if (SPC5_OSC_BYPASS == TRUE)
  /* If the board is equipped with an oscillator instead of a xtal then the
     bypass must be activated.*/
  CGM.FXOSC_CTL.B.OSCBYP = TRUE;
#endif /* SPC5_OSC_BYPASS */

  /* Setting the various dividers and source selectors.*/
  CGM.SC_DC0.R     = SPC5_CGM_SC_DC0;
  CGM.SC_DC1.R     = SPC5_CGM_SC_DC1;
  CGM.SC_DC2.R     = SPC5_CGM_SC_DC2;

  /* Initialization of the FMPLLs settings.*/
  CGM.FMPLL_CR.R = SPC5_FMPLL_ODF |
                   SPC5_FMPLL_IDF(SPC5_FMPLL_IDF_VALUE) |
                   SPC5_FMPLL_NDIV(SPC5_FMPLL_NDIV_VALUE) |
                   SPC5_FMPLL_OPTIONS;
  CGM.FMPLL_MR.R = SPC5_FMPLL_MR_INIT;

  /* Run modes initialization.*/
  ME.IS.R           = 8;                        /* Resetting I_ICONF status.*/
  ME.MER.R          = SPC5_ME_ME_BITS;          /* Enabled run modes.       */
  ME.TEST.R         = SPC5_ME_TEST_MC_BITS;     /* TEST run mode.           */
  ME.SAFE.R         = SPC5_ME_SAFE_MC_BITS;     /* SAFE run mode.           */
  ME.DRUN.R         = SPC5_ME_DRUN_MC_BITS;     /* DRUN run mode.           */
  ME.RUN[0].R       = SPC5_ME_RUN0_MC_BITS;     /* RUN0 run mode.           */
  ME.RUN[1].R       = SPC5_ME_RUN1_MC_BITS;     /* RUN1 run mode.           */
  ME.RUN[2].R       = SPC5_ME_RUN2_MC_BITS;     /* RUN2 run mode.           */
  ME.RUN[3].R       = SPC5_ME_RUN3_MC_BITS;     /* RUN0 run mode.           */
  ME.HALT.R         = SPC5_ME_HALT_MC_BITS;     /* HALT run mode.           */
  ME.STOP.R         = SPC5_ME_STOP_MC_BITS;     /* STOP run mode.           */
  ME.STANDBY.R      = SPC5_ME_STANDBY_MC_BITS;  /* STANDBY run mode.        */
  if ((ME.IS.B.I_ICONF & 1U)== 1U) {
    /* Configuration rejected.*/
    SPC5_CLOCK_FAILURE_HOOK();
  }

  /* Peripherals run and low power modes initialization.*/
  ME.RUNPC[0].R     = SPC5_ME_RUN_PC0_BITS;
  ME.RUNPC[1].R     = SPC5_ME_RUN_PC1_BITS;
  ME.RUNPC[2].R     = SPC5_ME_RUN_PC2_BITS;
  ME.RUNPC[3].R     = SPC5_ME_RUN_PC3_BITS;
  ME.RUNPC[4].R     = SPC5_ME_RUN_PC4_BITS;
  ME.RUNPC[5].R     = SPC5_ME_RUN_PC5_BITS;
  ME.RUNPC[6].R     = SPC5_ME_RUN_PC6_BITS;
  ME.RUNPC[7].R     = SPC5_ME_RUN_PC7_BITS;
  ME.LPPC[0].R      = SPC5_ME_LP_PC0_BITS;
  ME.LPPC[1].R      = SPC5_ME_LP_PC1_BITS;
  ME.LPPC[2].R      = SPC5_ME_LP_PC2_BITS;
  ME.LPPC[3].R      = SPC5_ME_LP_PC3_BITS;
  ME.LPPC[4].R      = SPC5_ME_LP_PC4_BITS;
  ME.LPPC[5].R      = SPC5_ME_LP_PC5_BITS;
  ME.LPPC[6].R      = SPC5_ME_LP_PC6_BITS;
  ME.LPPC[7].R      = SPC5_ME_LP_PC7_BITS;

  /* CFLASH settings calculated for a maximum clock of 48MHz.*/
  CFLASH.PFCR0.B.BK0_APC  = 2;
  CFLASH.PFCR0.B.BK0_RWSC = 2;

  /* Switches again to DRUN mode (current mode) in order to update the
     settings.*/
  if (SPCSetRunMode(SPC5_RUNMODE_DRUN) == CLOCK_FAILED) {
    SPC5_CLOCK_FAILURE_HOOK();
  }

  /* Set the API/RTC clock if the API/RTC is enabled.*/
#if (SPC5_ENABLE_API_RTC == TRUE)
  SPCSetPeripheralClockMode(91,
                            SPC5_ME_PCTL_RUN(1) | SPC5_ME_PCTL_LP(2));

  /* API/RTC clock selector may be updated only when CNTEN is 0.*/
  if (RTC.RTCC.B.CNTEN == 0U) {
    RTC.RTCC.B.CLKSEL = (uint8_t)(SPC5_API_RTC_CLK_SRC);
  }
#endif

  /* Clock Out selection after all the other configurations.*/
  CGM.OCDS_SC.R  = SPC5_CLKOUT_SRC | SPC5_CGM_OCDS_SELDIV;
#endif /* !SPC5_NO_INIT */
}

/**
 * @brief   Early initialization code.
 * @note    This initialization must be performed just after stack setup and
 *          before any other initialization.
 *
 * @special
 */
void __early_init(void) {

 spc_clock_init();
}

/**
 * @brief   Switches the system to the specified run mode.
 *
 * @param[in] mode      one of the possible run modes
 *
 * @return              The operation status.
 * @retval CLOCK_SUCCESS if the switch operation has been completed.
 * @retval CLOCK_FAILED  if the switch operation failed.
 */
uint8_t SPCSetRunMode(spc5_runmode_t mode) {

  /* Clearing status register bits I_IMODE(4) and I_IMTC(1).*/
  ME.IS.R = 5;

  /* Starts a transition process.*/
  ME.MCTL.R = SPC5_ME_MCTL_MODE(mode) | SPC5_ME_MCTL_KEY;
  ME.MCTL.R = SPC5_ME_MCTL_MODE(mode) | SPC5_ME_MCTL_KEY_INV;

  /* Waits for the mode switch or an error condition.*/
  for ( ; ; ) {
    uint32_t r = ME.IS.R;
    if ((r & 1U) == 1U) {
      return CLOCK_SUCCESS;
    }
    if ((r & 4U) == 4U) {
      return CLOCK_FAILED;
    }
  }
}

/**
 * @brief   Changes the clock mode of a peripheral.
 *
 * @param[in] n         index of the @p PCTL register
 * @param[in] pctl      new value for the @p PCTL register
 *
 * @notapi
 */
void SPCSetPeripheralClockMode(uint32_t n, uint32_t pctl) {
  uint32_t mode;

  ME.PCTL[n].R = (uint8_t)pctl;
  mode = ME.MCTL.B.TARGET_MODE;
  ME.MCTL.R = SPC5_ME_MCTL_MODE(mode) | SPC5_ME_MCTL_KEY;
  ME.MCTL.R = SPC5_ME_MCTL_MODE(mode) | SPC5_ME_MCTL_KEY_INV;
}

#if (SPC5_NO_INIT == FALSE) || defined(__DOXYGEN__)
/**
 * @brief   Returns the system clock under the current run mode.
 *
 * @return              The system clock in Hertz.
 */
uint32_t SPCGetSystemClock(void) {
  uint32_t sysclk;

  sysclk = ME.GS.B.S_SYSCLK;
  switch (sysclk) {
  case SPC5_ME_GS_SYSCLK_FIRC:
    return SPC5_FIRC_CLK;
  case SPC5_ME_GS_SYSCLK_FIRCDIV:
    return SPC5_FIRC_CLK / SPC5_FIRCDIV_VALUE;
  case SPC5_ME_GS_SYSCLK_XOSC:
    return SPC5_XOSC_CLK;
  case SPC5_ME_GS_SYSCLK_XOSCDIV:
    return SPC5_XOSC_CLK / SPC5_XOSCDIV_VALUE;
  case SPC5_ME_GS_SYSCLK_FMPLL:
    return SPC5_FMPLL_CLK;
  default:
    return 0;
  }
}
#endif /* !SPC5_NO_INIT */

/** @} */
