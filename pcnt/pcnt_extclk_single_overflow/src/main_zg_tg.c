/***************************************************************************//**
 * @file
 * @brief This project demonstrates the single external clock input oversampling
 *        mode of the pulse counter with interrupts. In this example, Push Button
 *        PB0 is the clock source and each clock cycle is generated by the press 
 *        and release of the button. The program generates an interrupt whenever the 
 *        pulse counter detects the number of pulses that went above a threshold set by user.
 *        This source file is for the zero gecko and tiny gecko.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_prs.h"

#include "bsp.h"

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowleges the interrupt request and toggles LED0
 ******************************************************************************/ 
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_OF);
  
  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief Initialize PCNT
 *        This function initializes pulse counter 0 and sets up the external
 *        single mode PCNT0 overflow interrupt is configured in this
 *        function also. 
 ******************************************************************************/ 
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtSingle;        // Use External Clock sorce from S0
  pcntInit.top      = 5;                        // Overflow after every 6th press
  pcntInit.s1CntDir = false;                    // S1 does not affect counter direction, default count up
  pcntInit.s0PRS    = pcntPRSCh0;

  /* Init PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable PRS Channel 0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable Interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_OF);
}

/***************************************************************************//**
 * @brief Initialize PRS
 *        This function sets up the PRS to GPIO pin 8, which is used to wire
 *        PF6 to PCNT0 PRS0
 ******************************************************************************/ 
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Link PRS Channel 0 to GPIO PIN 8 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN8);
}

/***************************************************************************//**
 * @brief Initialize GPIO
 *        This function initializes push button PB0 and enable external 
 *        interrupt for PRS functionality
 ******************************************************************************/ 
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - BTN0 on PF6 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure external interrupt for BTN0 */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 8, false, false, false);
}

/***************************************************************************//**
 * @brief Initialize NVIC
 *        This function enables PCNT0 interrupt requests in the
 *        interrupt controller
 ******************************************************************************/ 
static void setupNvic(void)
{
  /* Clear PCNT0 pending interrupt */
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* Initialize GPIO */
  setupGpio();

  /* Initialize PRS */
  setupPrs();

  /* Initialize PCNT */
  setupPcnt();

  /* Initialize NVIC */
  setupNvic();

  /* Enter EM3 forever */
  while (true) {
    EMU_EnterEM3(false);
  }
}
