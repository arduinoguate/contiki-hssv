//+------------------------------------------------------------------------------------------------+
//| Kinetis MKL26 microcontroller initialization code.                                             |
//|                                                                                                |
//| This file implements very basic microcontroller code infrastructure such as the vector table,  |
//| processor and peripheral initialization, memory initialization and non-volatile configuration  |
//| bits.                                                                                          |
//|                                                                                                |
//| Author: Joksan Alvarado.                                                                       |
//+------------------------------------------------------------------------------------------------+

#include <stdint.h>

#include "mkl26-sim.h"

//Interrupt vector handler function type.
typedef void (* handler_t)();

//Flash configuration field structure.
struct flash_configuration_field_type {
  uint8_t backdoor_comparison_key[8];
  uint8_t FPROT[4];
  uint8_t FSEC;
  uint8_t FOPT;
  uint8_t reserved0;
  uint8_t reserved1;
};

//Symbols exported by the linker script.
extern uint32_t __stack_end__;
extern const uint32_t __relocate_flash_start__;
extern uint32_t __relocate_sram_start__;
extern uint32_t __relocate_sram_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

//External functions invoked by startup code.
extern void __libc_init_array();
extern void main();

//--------------------------------------------------------------------------------------------------

//Startup routine, located at reset vector.
void startup() {
  const uint32_t *flash;
  uint32_t *sram;

  //Disable the watchdog.
  SIM->COPC = SIM_COPC_COPW_Normal | SIM_COPC_COPCLKS_Int_1kHz | SIM_COPC_COPT_Disabled;

  //TODO: Initialize all clocks.

  //Enable the clocks of all ports.
  SIM->SCGC5 = SIM_SCGC5_PORTA_Enabled | SIM_SCGC5_PORTB_Enabled | SIM_SCGC5_PORTC_Enabled |
               SIM_SCGC5_PORTD_Enabled | SIM_SCGC5_PORTE_Enabled;

  //Copy the initial data for the data section from FLASH to RAM.
  flash = &__relocate_flash_start__;
  sram = &__relocate_sram_start__;
  while (sram < &__relocate_sram_end__)
    *sram++ = *flash++;

  //Initialize the .bss section to zeroes.
  sram = &__bss_start__;
  while (sram < &__bss_end__)
    *sram++ = 0;

  //Initialize libc.
  __libc_init_array();

  //System is up. Call the main function.
  main();
}

//Default interrupt handler.
static void unused_handler() {
  //The default unused handler does nothing, just stalls the CPU.
  for (;;);
}

//--------------------------------------------------------------------------------------------------

//Weak references for core system handler vectors.
void __attribute__((weak, alias("unused_handler"))) nmi_handler();
void __attribute__((weak, alias("unused_handler"))) hard_fault_handler();
void __attribute__((weak, alias("unused_handler"))) svcall_handler();
void __attribute__((weak, alias("unused_handler"))) pendablesrvreq_handler();
void __attribute__((weak, alias("unused_handler"))) systick_handler();

//Weak references for non-core (peripheral) vectors.
void __attribute__((weak, alias("unused_handler"))) dma_channel_0_handler();
void __attribute__((weak, alias("unused_handler"))) dma_channel_1_handler();
void __attribute__((weak, alias("unused_handler"))) dma_channel_2_handler();
void __attribute__((weak, alias("unused_handler"))) dma_channel_3_handler();
void __attribute__((weak, alias("unused_handler"))) flash_memory_module_handler();
void __attribute__((weak, alias("unused_handler"))) low_voltage_handler();
void __attribute__((weak, alias("unused_handler"))) low_leakage_wakeup_handler();
void __attribute__((weak, alias("unused_handler"))) i2c_0_handler();
void __attribute__((weak, alias("unused_handler"))) i2c_1_handler();
void __attribute__((weak, alias("unused_handler"))) spi_0_handler();
void __attribute__((weak, alias("unused_handler"))) spi_1_handler();
void __attribute__((weak, alias("unused_handler"))) uart_0_handler();
void __attribute__((weak, alias("unused_handler"))) uart_1_handler();
void __attribute__((weak, alias("unused_handler"))) uart_2_handler();
void __attribute__((weak, alias("unused_handler"))) adc_0_handler();
void __attribute__((weak, alias("unused_handler"))) cmp_0_handler();
void __attribute__((weak, alias("unused_handler"))) tpm_0_handler();
void __attribute__((weak, alias("unused_handler"))) tpm_1_handler();
void __attribute__((weak, alias("unused_handler"))) tpm_2_handler();
void __attribute__((weak, alias("unused_handler"))) rtc_alarm_handler();
void __attribute__((weak, alias("unused_handler"))) rtc_seconds_handler();
void __attribute__((weak, alias("unused_handler"))) pit_handler();
void __attribute__((weak, alias("unused_handler"))) i2s_0_handler();
void __attribute__((weak, alias("unused_handler"))) usb_otg_handler();
void __attribute__((weak, alias("unused_handler"))) dac_0_handler();
void __attribute__((weak, alias("unused_handler"))) tsi_0_handler();
void __attribute__((weak, alias("unused_handler"))) mcg_handler();
void __attribute__((weak, alias("unused_handler"))) lptmr_0_handler();
void __attribute__((weak, alias("unused_handler"))) port_a_handler();
void __attribute__((weak, alias("unused_handler"))) port_c_d_handler();

//--------------------------------------------------------------------------------------------------

//Processor vector table, located at 0x00000000.
static __attribute__ ((section(".vectors"), used))
handler_t vectors[48] = {
  //Core system handler vectors.
  (handler_t) &__stack_end__,   //0 - Initial stack pointer
  startup,                      //1 - Initial program counter
  nmi_handler,                  //2 - Non maskable interrupt
  hard_fault_handler,           //3 - Hard fault
  unused_handler,               //4
  unused_handler,               //5
  unused_handler,               //6
  unused_handler,               //7
  unused_handler,               //8
  unused_handler,               //9
  unused_handler,               //10
  svcall_handler,               //11 - Supervisor call
  unused_handler,               //12
  unused_handler,               //13
  pendablesrvreq_handler,       //14 - Pendable request for system service
  systick_handler,              //15 - System tick timer

  //Non-core vectors.
  dma_channel_0_handler,            //16 - DMA channel 0 transfer complete and error
  dma_channel_1_handler,            //17 - DMA channel 1 transfer complete and error
  dma_channel_2_handler,            //18 - DMA channel 2 transfer complete and error
  dma_channel_3_handler,            //19 - DMA channel 3 transfer complete and error
  unused_handler,                   //20
  flash_memory_module_handler,      //21 - Flash memory module command complete and read collision
  low_voltage_handler,              //22 - Low voltage detect and low voltage warning interrupt
  low_leakage_wakeup_handler,       //23 - Low leakage wake up
  i2c_0_handler,                    //24 - I2C 0
  i2c_1_handler,                    //25 - I2C 1
  spi_0_handler,                    //26 - SPI 0
  spi_1_handler,                    //27 - SPI 1
  uart_0_handler,                   //28 - UART 0 status and error
  uart_1_handler,                   //29 - UART 1 status and error
  uart_2_handler,                   //30 - UART 2 status and error
  adc_0_handler,                    //31 - ADC 0
  cmp_0_handler,                    //32 - CMP 0
  tpm_0_handler,                    //33 - TPM 0
  tpm_1_handler,                    //34 - TPM 1
  tpm_2_handler,                    //35 - TPM 2
  rtc_alarm_handler,                //36 - RTC alarm interrupt
  rtc_seconds_handler,              //37 - RTC seconds interrupt
  pit_handler,                      //38 - PIT (all channels)
  i2s_0_handler,                    //39 - I2S
  usb_otg_handler,                  //40 - USB OTG
  dac_0_handler,                    //41 - DAC 0
  tsi_0_handler,                    //42 - TSI 0
  mcg_handler,                      //43 - MCG
  lptmr_0_handler,                  //44 - Low power timer
  unused_handler,                   //45
  port_a_handler,                   //46 - Port A pin detect
  port_c_d_handler,                 //47 - Port C and D pin detect
};

//--------------------------------------------------------------------------------------------------

//Flash configuration field instance, located at 0x00000400.
static __attribute__ ((section(".flash_configuration_field"), used))
struct flash_configuration_field_type flash_config = {
  .backdoor_comparison_key = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
  .FPROT = { 0xFF, 0xFF, 0xFF, 0xFF, },
  .FSEC = 0xFE,     //Disable backdoor access and security, enable mass erase and factory access.
  .FOPT = 0xFB,     //Fast initialization, RESET_b as reset, disable NMI, OUTDIV1 is 0 (high speed)
  .reserved0 = 0xFF,
  .reserved1 = 0xFF,
};
