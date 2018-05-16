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

#include "components.h"


/*
 * Prevent linking some relocation stuff function from libgcc called implicitly by main()
 */
void __eabi(void) {

}

/*
 * Application entry point.
 */
int main(void) {

  /* Initialization of all the imported components in the order specified in
     the application wizard. The function is generated automatically.*/
  boardInit();
  irqInit();
  eirqInit();
  clockInit();

  /* PIT clock enabled. */
  SPCSetPeripheralClockMode(92, SPC5_ME_PCTL_RUN(2) | SPC5_ME_PCTL_LP(2));
  PIT.PITMCR.R      = 1;

  /* Enable PIT1 Timer for Interrupt generation. */
  PIT.CH[1].LDVAL.R = 0x005DC6C0; /* 1 second value using 48MHZ clock         */
  PIT.CH[1].CVAL.R  = 0x005DC6C0; /* 1 second value using 48MHZ clock         */
  PIT.CH[1].TFLG.R  = 1;         /* Interrupt flag cleared.                  */
  PIT.CH[1].TCTRL.R = 3;         /* Timer active, interrupt enabled.         */

  // Stupid loop used to debug openocd step/bkpt
  //void debug_step(void);
  //debug_step();

  /* Interrupts are enabled and the system is ready.*/
  irqIsrEnable();

  pal_togglepad(PORT_C, PC_LED7);

  /* Application main loop.*/
  for ( ; ; ) {

  }
}
