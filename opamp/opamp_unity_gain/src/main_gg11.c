/**************************************************************************//**
 * @main_gg11.c
 * @brief This project configures opamp 2 as a voltage follower with unity
 * gain. The output is routed back to the inverting input and the output voltage
 * is equal to the input voltage in value. This configuration is commonly used
 * as a buffer to increase drive strength and drive higher loads. This project
 * operates in EM3.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_opamp.h"

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No APORT signal is explicitly routed to the negative input of the opamp
 *    because the default OPA_INIT_UNITY_GAIN macro already takes care of
 *    routing the output back to the negative input (as defined by a voltage
 *    follower).
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Define the configuration for OPA2
  OPAMP_Init_TypeDef init =  OPA_INIT_UNITY_GAIN;
  init.posSel  = opaPosSelAPORT3XCH10;  // Choose opamp positive input to come from PE10
  init.outMode = opaOutModeAPORT3YCH11; // Route opamp output to PE11

  // Enable OPA2
  OPAMP_Enable(VDAC0, OPA2, &init);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

