/*
 * This file is part of the stm32-msc-bootloader project.
 *
 * Copyright (C) 2018 Michel Stempin <michel.stempin@wanadoo.fr>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/msc.h>
#include <libopencm3/cm3/scb.h>
#include "pseudo_fat.h"

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0110,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5741,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const struct usb_endpoint_descriptor msc_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,
}};

static const struct usb_interface_descriptor msc_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_MSC,
	.bInterfaceSubClass = USB_MSC_SUBCLASS_SCSI,
	.bInterfaceProtocol = USB_MSC_PROTOCOL_BBB,
	.iInterface = 0,
	.endpoint = msc_endp,
	.extra = NULL,
	.extralen = 0
}};

static const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = msc_iface,
}};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,
	.interface = ifaces,
};

static const char *usb_strings[] = {
	"http://www.stm32duino.com",
	"MSC Bootloader",
	"BluePill",
};

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[128];

int main(void)
{
	static usbd_device *usbd_dev;

	//SCB_VTOR = (uint32_t) 0x08002000;

	/* RCC Set System Clock PLL at 72MHz from HSE at 8MHz */
	/* Enable internal high-speed oscillator. */
	RCC_CR |= RCC_CR_HSION;
	while (!(RCC_CR & RCC_CR_HSIRDY));

	/* Select HSI as SYSCLK source. */
	RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW) |
	  (RCC_CFGR_SW_SYSCLKSEL_HSICLK << RCC_CFGR_SW_SHIFT);

	/* Enable external high-speed oscillator 8MHz. */
	RCC_CR |= RCC_CR_HSEON;
	while (!(RCC_CR & RCC_CR_HSERDY));

	/*
	 * Set prescalers for AHB, ADC, ABP1, ABP2.
	 * Do this before touching the PLL (TODO: why?).
	 */
	RCC_CFGR = ((RCC_CFGR &
		     ~(RCC_CFGR_SW |
		       RCC_CFGR_HPRE |
		       RCC_CFGR_ADCPRE |
		       RCC_CFGR_PPRE1 |
		       RCC_CFGR_PPRE2)) |
		    ((RCC_CFGR_SW_SYSCLKSEL_HSECLK << RCC_CFGR_SW_SHIFT) |
		     (RCC_CFGR_HPRE_SYSCLK_NODIV << RCC_CFGR_HPRE_SHIFT) |
		     (RCC_CFGR_ADCPRE_PCLK2_DIV8 << RCC_CFGR_ADCPRE_SHIFT) |
		     (RCC_CFGR_PPRE1_HCLK_DIV2 << RCC_CFGR_PPRE1_SHIFT) |
		     (RCC_CFGR_PPRE2_HCLK_NODIV << RCC_CFGR_PPRE2_SHIFT)));

	/*
	 * Sysclk runs with 72MHz -> 2 waitstates.
	 * 0WS from 0-24MHz
	 * 1WS from 24-48MHz
	 * 2WS from 48-72MHz
	 */
	FLASH_ACR = ((FLASH_ACR &
		      ~(FLASH_ACR_LATENCY_MASK << FLASH_ACR_LATENCY_SHIFT)) |
		     (FLASH_ACR_LATENCY_2WS << FLASH_ACR_LATENCY_SHIFT));

	/*
	 * Set the PLL multiplication factor to 9.
	 * 8MHz (external) * 9 (multiplier) = 72MHz
	 *
	 * Select HSE as PLL source.
	 */
	RCC_CFGR = ((RCC_CFGR &
		    ~(RCC_CFGR_PLLMUL |
		      RCC_CFGR_PLLSRC |
		      RCC_CFGR_PLLXTPRE)) |
	  ((RCC_CFGR_PLLMUL_PLL_CLK_MUL9 << RCC_CFGR_PLLMUL_SHIFT) |
	   (RCC_CFGR_PLLSRC_HSE_CLK << 16) |
	   (RCC_CFGR_PLLXTPRE_HSE_CLK << 17)));


	/*
	 * External frequency undivided before entering PLL
	 * (only valid/needed for HSE).
	 */

	/* Enable PLL oscillator and wait for it to stabilize. */
	RCC_CR |= RCC_CR_PLLON;
	while (!(RCC_CR & RCC_CR_PLLRDY));

	/* Select PLL as SYSCLK source. */
	RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW) |
	  (RCC_CFGR_SW_SYSCLKSEL_PLLCLK << RCC_CFGR_SW_SHIFT);

	/* Enable clocks for GPIOA and GPIOC */
	RCC_APB2ENR |= (1 << 2) | (1 << 4);

	/* Setup GPIOC Pin 12 to pull up the D+ high, so autodect works
	 * with the bootloader.  The circuit is active low. */
	//gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
	//	      GPIO_CNF_OUTPUT_OPENDRAIN, GPIO12);
	GPIOC_CRH = ((GPIOC_CRH & (0xf << 16)) |
		     (GPIO_MODE_OUTPUT_2_MHZ << 16) |
		     (GPIO_CNF_OUTPUT_OPENDRAIN << 18));
	GPIO_BSRR(GPIOC) = (GPIO12 << 16);

	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config,
			     usb_strings, 3, usbd_control_buffer,
			     sizeof (usbd_control_buffer));

	pseudo_fat_init();
	usb_msc_init(usbd_dev, 0x82, 64, 0x01, 64, "BluePill", "stm32duino.com",
		     "0.01", TOTAL_SECTORS, pseudo_fat_read, pseudo_fat_write);

	while (1) {
		usbd_poll(usbd_dev);
	}
}
