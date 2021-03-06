/* This file is part of the libopencm3 project.
 *
 * It was generated by the irq2nvic_h script.
 */

#ifndef LIBOPENCM3_SAM3A_NVIC_H
#define LIBOPENCM3_SAM3A_NVIC_H

#include <libopencm3/cm3/nvic.h>

/** @defgroup CM3_nvic_defines_SAM3A User interrupts for Atmel SAM3A series
    @ingroup CM3_nvic_defines

    @{*/

#define NVIC_SUPC_IRQ 0
#define NVIC_RSTC_IRQ 1
#define NVIC_RTC_IRQ 2
#define NVIC_RTT_IRQ 3
#define NVIC_WDT_IRQ 4
#define NVIC_PMC_IRQ 5
#define NVIC_EEFC0_IRQ 6
#define NVIC_EEFC1_IRQ 7
#define NVIC_UART_IRQ 8
#define NVIC_SMC_SDRAMC_IRQ 9
#define NVIC_SDRAMC_IRQ 10
#define NVIC_PIOA_IRQ 11
#define NVIC_PIOB_IRQ 12
#define NVIC_PIOC_IRQ 13
#define NVIC_PIOD_IRQ 14
#define NVIC_PIOE_IRQ 15
#define NVIC_PIOF_IRQ 16
#define NVIC_USART0_IRQ 17
#define NVIC_USART1_IRQ 18
#define NVIC_USART2_IRQ 19
#define NVIC_USART3_IRQ 20
#define NVIC_HSMCI_IRQ 21
#define NVIC_TWI0_IRQ 22
#define NVIC_TWI1_IRQ 23
#define NVIC_SPI0_IRQ 24
#define NVIC_SPI1_IRQ 25
#define NVIC_SSC_IRQ 26
#define NVIC_TC0_IRQ 27
#define NVIC_TC1_IRQ 28
#define NVIC_TC2_IRQ 29
#define NVIC_TC3_IRQ 30
#define NVIC_TC4_IRQ 31
#define NVIC_TC5_IRQ 32
#define NVIC_TC6_IRQ 33
#define NVIC_TC7_IRQ 34
#define NVIC_TC8_IRQ 35
#define NVIC_PWM_IRQ 36
#define NVIC_ADC_IRQ 37
#define NVIC_DACC_IRQ 38
#define NVIC_DMAC_IRQ 39
#define NVIC_UOTGHS_IRQ 40
#define NVIC_TRNG_IRQ 41
#define NVIC_RESERVED0_IRQ 42
#define NVIC_CAN0_IRQ 43
#define NVIC_CAN1_IRQ 44

#define NVIC_IRQ_COUNT 45

/**@}*/

/** @defgroup CM3_nvic_isrprototypes_SAM3A User interrupt service routines (ISR) prototypes for Atmel SAM3A series
    @ingroup CM3_nvic_isrprototypes

    @{*/

BEGIN_DECLS

void supc_isr(void);
void rstc_isr(void);
void rtc_isr(void);
void rtt_isr(void);
void wdt_isr(void);
void pmc_isr(void);
void eefc0_isr(void);
void eefc1_isr(void);
void uart_isr(void);
void smc_sdramc_isr(void);
void sdramc_isr(void);
void pioa_isr(void);
void piob_isr(void);
void pioc_isr(void);
void piod_isr(void);
void pioe_isr(void);
void piof_isr(void);
void usart0_isr(void);
void usart1_isr(void);
void usart2_isr(void);
void usart3_isr(void);
void hsmci_isr(void);
void twi0_isr(void);
void twi1_isr(void);
void spi0_isr(void);
void spi1_isr(void);
void ssc_isr(void);
void tc0_isr(void);
void tc1_isr(void);
void tc2_isr(void);
void tc3_isr(void);
void tc4_isr(void);
void tc5_isr(void);
void tc6_isr(void);
void tc7_isr(void);
void tc8_isr(void);
void pwm_isr(void);
void adc_isr(void);
void dacc_isr(void);
void dmac_isr(void);
void uotghs_isr(void);
void trng_isr(void);
void reserved0_isr(void);
void can0_isr(void);
void can1_isr(void);

END_DECLS

/**@}*/

#endif /* LIBOPENCM3_SAM3A_NVIC_H */
