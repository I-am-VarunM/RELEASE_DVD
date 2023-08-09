/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "sysmon_header.h"
#include "sysmonpsu_header.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xgpiops.h"
#include "scugic_header.h"
#include "xscugic.h"
#include "xsysmonpsu.h"
#include "xsysmon.h"
#include "gpio_header.h"
#include "system_test.h"
#include "xtime_l.h"
#include "uart_header.h"
#include "pmodtmp2.h"
#include <stdlib.h>
#include "sleep.h"
//#include "feature_extraction.h"
//#include "mlmodel.h"
//#define RUN_SUBMODULES

//Set global constants
#define CHANNEL_LED 				1
#define CHANNEL_PUSH 				2
#define CHANNEL_DIP 				1
#define CHANNEL_LOAD_IP_CORE_EN 	2
#define CHANNEL_GPIO_OUT_LOAD_EN 	2
#define CHANNEL_GPIO_OUT_DUTY_CYCLE 1
#define BTN_MEAS_PAUSE				0x04
#define BTN_MEAS_STOP				0x08
#define LED_LOAD_STARTED			0x01
#define LED_MEAS_PAUSED				0x04
#define STATUS_LOAD_ON 				0x00
#define STATUS_LOAD_OFF 			0x01
#define MEASUREMENT_TYPE_AMBIENT_TEMP 	0x00
#define MEASUREMENT_TYPE_PSU_TEMP 		0x01
#define MEASUREMENT_TYPE_PSU_REM_TEMP 	0x02
#define MEASUREMENT_TYPE_PL_TEMP 		0x03
#define MEASUREMENT_TYPE_PSU_VCCINT		0x04
#define MEASUREMENT_TYPE_PSU_VCCAUX 	0x05
#define MEASUREMENT_TYPE_PL_VCCINT 		0x06
#define MEASUREMENT_TYPE_PL_VCCAUX 		0x07

#define AMOUNT_OF_COARSE_MODULES 0
#define AMOUNT_OF_FINE_MODULES 32
#define COARSE_MODULES_BIT_MASK (0xFFFFFFFF >> (32-AMOUNT_OF_COARSE_MODULES))
#define FINE_MODULES_BIT_MASK (0xFFFFFFFF >> (32-AMOUNT_OF_FINE_MODULES))
#define NEW_DUTY_CYCLE_ENABLE_BIT 0x100
#define WAIT_TILL_STARTING_LOAD_TIME_INTERVAL_SEC 10
#define WAIT_TILL_STOPPING_LOAD_TIME_INTERVAL_SEC 30
#define WAIT_TILL_ENDING_MEAS_TIME_INTERVAL_SEC  10
#define WAIT_TILL_STARTING_LOAD_TIME_INTERVAL_MS 10000
#define WAIT_TILL_STOPPING_LOAD_TIME_INTERVAL_MS 30000
#define WAIT_TILL_ENDING_MEAS_TIME_INTERVAL_MS  10000
#define TOTAL_MEAS_TIME_INTERVAL_SEC  (WAIT_TILL_STARTING_LOAD_TIME_INTERVAL_SEC + WAIT_TILL_STOPPING_LOAD_TIME_INTERVAL_SEC + WAIT_TILL_ENDING_MEAS_TIME_INTERVAL_SEC)
#define TOTAL_MEAS_TIME_INTERVAL_MS  (WAIT_TILL_STARTING_LOAD_TIME_INTERVAL_MS + WAIT_TILL_STOPPING_LOAD_TIME_INTERVAL_MS + WAIT_TILL_ENDING_MEAS_TIME_INTERVAL_MS)
//#define TOTAL_MEAS_TIME_INTERVAL_MS 1000

#define SEC_TO_TIMESTAMP_FACTOR  100000000 // 100000000 = 1 sec
#define MS_TO_TIMESTAMP_FACTOR  100000 // 100000000 = 1 sec
#define MEASUREMENT_OFFSET 1000
#define MEASUREMENT_TIME_OFFSET_MS (70)

int load_status;
int button1_on_history;
int dip_or_load_history;
int polling_paused_history;
int loadStarted;
void sendStatusMsgViaUart(int status, int value);
void triggerMeasurementLoad(XGpio * gpio_led_pb_handler, XGpio * gpio_load_handler, XGpio * gpio_led_handler);


void setup(XGpio* my_gpios_led_pb, XGpio* my_gpios_dip_ipload,
		XGpio* my_gpios_load) {
	XTime_StartTimer();
	//Setup UART Interrupt
	int status = UartPsIntrInitialize(&InterruptController, &UartPs,
			UART_DEVICE_ID, UART_INT_IRQ_ID);
	if (status != XST_SUCCESS) {
		printf("UART Interrupt Example Test Failed\r\n");
		//return XST_FAILURE;
	}
	printf("UART Interrupt Setup Successful\r\n");
	//Setup GPIO Init PL
	XGpio_Initialize(&*my_gpios_led_pb, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_Initialize(&*my_gpios_load, XPAR_AXI_GPIO_1_DEVICE_ID);
	XGpio_Initialize(&*my_gpios_dip_ipload, XPAR_AXI_GPIO_2_DEVICE_ID);
	//Run GPIO Self Test
	int result = XGpio_SelfTest(&*my_gpios_led_pb);
	if (result != XST_SUCCESS) {
		print("Gpio SelfTest failed!\r");
	}
	result = XGpio_SelfTest(&*my_gpios_dip_ipload);
	if (result != XST_SUCCESS) {
		print("Gpio SelfTest failed!\r");
	}
	result = XGpio_SelfTest(&*my_gpios_load);
	if (result != XST_SUCCESS) {
		print("Gpio SelfTest failed!\r");
	}
	//Setup Ambient Temp Sensor
//	result = configurePmodtmp2();
//	if (result != XST_SUCCESS) {
//		print("Setup Ambient Temp Sensor failed!\r");
//	}
	//Set Data Directions
	XGpio_SetDataDirection(&*my_gpios_led_pb, CHANNEL_PUSH, 0xffffffff); //Bits set to 0 are output and bits set to 1 are input.
	XGpio_SetDataDirection(&*my_gpios_led_pb, CHANNEL_LED, 0x0); //Bits set to 0 are output and bits set to 1 are input.
	XGpio_SetDataDirection(&*my_gpios_dip_ipload, CHANNEL_DIP, 0xffffffff); //Bits set to 0 are output and bits set to 1 are input.
	//XGpio_SetDataDirection(&*my_gpios_dip_ipload, CHANNEL_LOAD_IP_CORE_EN, 0x0); //Bits set to 0 are output and bits set to 1 are input.
	//XGpio_SetDataDirection(&*my_gpios_load, CHANNEL_GPIO_OUT_DUTY_CYCLE, 0x0); //Bits set to 0 are output and bits set to 1 are input.
	XGpio_SetDataDirection(&*my_gpios_load, CHANNEL_GPIO_OUT_LOAD_EN, 0x0); //Bits set to 0 are output and bits set to 1 are input.
	//Setup PWM
	XGpio_DiscreteWrite(&*my_gpios_led_pb, CHANNEL_LED,
			 LED_MEAS_PAUSED); // Turn on LED1
	XGpio_DiscreteWrite(&*my_gpios_led_pb, CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
}


int main()
{
    init_platform();
    XGpio my_gpios_led_pb;
    XGpio my_gpios_dip_ipload;
    XGpio my_gpios_load;
    button1_on_history = 0;
    setup(&my_gpios_led_pb, &my_gpios_dip_ipload, &my_gpios_load);
    load_status =0;
    polling_paused_history = 1;
    dip_or_load_history=0;
    loadStarted = 0;


    print("Hello World\n\r");
    printf("Hi  \n");
    for(long i= 0; i<1000000; i++)
    {
    	//print("Loading....");

    	triggerMeasurementLoad(&my_gpios_led_pb, &my_gpios_load, &my_gpios_led_pb);
    	sleep(0.1);
    }
    cleanup_platform();
    return 0;
}
void sendStatusMsgViaUart(int status, int value)
{
	XTime * Xtime = 0;
	char * msg = NULL;
	XTime_GetTime(Xtime);
	printf("Uart msg status %i\n", status);
	switch(status){
		case STATUS_LOAD_ON:
			asiprintf(&msg,"Load started (time: %lu).\n",(*Xtime));
			printf("Load Started (time: %lu) . \n", (*Xtime));
			break;
		case STATUS_LOAD_OFF:
			asiprintf(&msg,"Load stopped (time: %lu).\n",(*Xtime));
			printf("Load Stopped (time: %lu). \n", (*Xtime));
			break;
	}
	int size_local = strlen(msg);
	//sendUart(&UartPs, msg, size_local);
	free(msg);
}
void triggerMeasurementLoad(XGpio * gpio_led_pb_handler, XGpio * gpio_load_handler, XGpio * gpio_led_handler)
{
	//Disable/Enable RO
//int dip_or_load_current = XGpio_DiscreteRead(gpio_dip_handler,CHANNEL_DIP) | loadEnabled;
	//int led_current = XGpio_DiscreteRead(gpio_led_pb_handler,CHANNEL_LED);
	//button2_on = (XGpio_DiscreteRead(gpio_led_pb_handler,CHANNEL_PUSH) & BTN_MEAS_STOP);
	int button1_on_current = ((XGpio_DiscreteRead(gpio_led_pb_handler,CHANNEL_PUSH) & BTN_MEAS_PAUSE)>>2);
	//printf("Logic :%d \n",(button1_on_current != button1_on_history));

	int led_current = XGpio_DiscreteRead(gpio_led_handler,CHANNEL_LED);
	sleep(0.4);
	//printf("load current: %d \n",dip_or_load_current);
	if(button1_on_current == 1 && button1_on_history==0){
		loadStarted = !loadStarted;
		if(loadStarted)

			{sendStatusMsgViaUart(STATUS_LOAD_ON,0);
			printf("GPIOs Dips:. Enable Load\n");
			led_current |= LED_LOAD_STARTED;
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x01); // Enable ro
			usleep(250000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
			usleep(350000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x01); // Enable ro
			usleep(250000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
			usleep(350000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x01); // Enable ro
			usleep(250000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
			usleep(350000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x01); // Enable ro
			usleep(250000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
			usleep(350000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x01); // Enable ro
			usleep(250000);
			XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
			usleep(350000);
			}
		else
		{
			printf("load off \n");
						sendStatusMsgViaUart(STATUS_LOAD_OFF,0);
						printf("GPIOs Dips: %i. Disable Load \n");
						led_current &= (~LED_LOAD_STARTED);
						XGpio_DiscreteWrite(gpio_load_handler,CHANNEL_GPIO_OUT_LOAD_EN, 0x00); // Disable ro
		}
		}
	button1_on_history = button1_on_current;
	XGpio_DiscreteWrite(gpio_led_handler,CHANNEL_LED, led_current);
	sleep(1); // avoiding keyboard debouncing
}
