#define TESTAPP_GEN

/******************************************************************************
*
* Copyright (C) 2016-2017 Xilinx, Inc.  All rights reserved.
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
/****************************************************************************/
/**
*
* @file xsysmonpsu_polled_example.c
*
* This file contains a design example using the driver functions
* of the System Monitor driver. The example here shows the
* driver/device in polled mode to check the on-chip temperature and voltages.
*
* @note
*
* This examples also assumes that there is a STDIO device in the system.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- -----  -------- -----------------------------------------------------
* 1.0   kvn    12/15/15 First release
*       ms     04/05/17 Modified Comment lines in functions to
*                       recognize it as documentation block for doxygen
*                       generation.
* 2.3   ms    12/12/17 Added peripheral test support.
*       mn    03/08/18 Update code to run at higher frequency and remove sleep
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xsysmonpsu.h"
#include "xparameters.h"
#include "xstatus.h"
#include "stdio.h"
#include "xtime_l.h"
#include "uart_header.h"
#include "sysmonpsu_header.h"
#include "sysmon_header.h"
#include "xuartps.h"
#include <stdlib.h>

/************************** Constant Definitions ****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define SYSMON_DEVICE_ID 	XPAR_XSYSMONPSU_0_DEVICE_ID


/**************************** Type Definitions ******************************/


/***************** Macros (Inline Functions) Definitions ********************/

#define printf xil_printf /* Small foot-print printf function */

/************************** Function Prototypes *****************************/

//int SysMonPsuPolledPrintfExample(u16 SysMonDeviceId);
//int SysMonPsuPolledSetup(u16 SysMonDeviceId, XSysMonPsu *SysMonInstPtr);
//int SysMonPsuPolledReadCurrent(u16 SysMonDeviceId);
//int SysMonPsuPolledReadMinMax(u16 SysMonDeviceId);
//static int SysMonPsuFractionToInt(float FloatNum);
int psu_rem_temp_max;

/************************** Variable Definitions ****************************/

static XSysMonPsu SysMonInst;      /* System Monitor driver instance */
int Setup_Done_PSU = 0;
#ifndef TESTAPP_GEN

/****************************************************************************/
/**
*
* Main function that invokes the polled example in this file.
*
* @param	None.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note		None.
*
*****************************************************************************/
int main(void)
{

	int Status;

	/*
	 * Run the SysMonitor polled example, specify the Device ID that is
	 * generated in xparameters.h.
	 */
	Status = SysMonPsuPolledPrintfExample(SYSMON_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("SysMon PSU Polled Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran SysMon PSU Polled Example Test\r\n");
	return XST_SUCCESS;
}
#endif

/****************************************************************************/
/**
*
* This function performs setup for the System Monitor device using the
* driver APIs.
* This function does the following tasks:
*	- Initiate the System Monitor device driver instance
*	- Run self-test on the device
*	- Setup the sequence registers to continuously monitor on-chip
*	temperature, VCCINT and VCCAUX
*	- Setup configuration registers to start the sequence
*
* @param	SysMonDeviceId is the XPAR_<SYSMON_instance>_DEVICE_ID value
*		from xparameters.h.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note   	None
*
****************************************************************************/
int SysMonPsuPolledSetup(u16 SysMonDeviceId, XSysMonPsu * SysMonInstPtr)
{
	int Status;
	XSysMonPsu_Config *ConfigPtr;
	u64 IntrStatus;
	psu_rem_temp_max = 0;

	printf("\r\nEntering the SysMon PSU Polled Setup. \r\n");


	/* Initialize the SysMon PSU driver. */
	ConfigPtr = XSysMonPsu_LookupConfig(SysMonDeviceId);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	XSysMonPsu_CfgInitialize(SysMonInstPtr, ConfigPtr,
				ConfigPtr->BaseAddress);

	/* Self Test the System Monitor device */
	Status = XSysMonPsu_SelfTest(SysMonInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Disable the Channel Sequencer before configuring the Sequence
	 * registers.
	 */
	XSysMonPsu_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_SAFE, XSYSMON_PS);


	/* Disable all the alarms in the Configuration Register 1. */
	XSysMonPsu_SetAlarmEnables(SysMonInstPtr, 0x0, XSYSMON_PS);


	/*
	 * Setup the Averaging to be done for the channels in the
	 * Configuration 0 register as 16 samples:
	 */
	XSysMonPsu_SetAvg(SysMonInstPtr, XSM_AVG_16_SAMPLES, XSYSMON_PS);

	/*
	 * Setup the Sequence register for 1st Auxiliary channel
	 * Setting is:
	 *	- Add acquisition time by 6 ADCCLK cycles.
	 *	- Bipolar Mode
	 *
	 * Setup the Sequence register for 16th Auxiliary channel
	 * Setting is:
	 *	- Add acquisition time by 6 ADCCLK cycles.
	 *	- Unipolar Mode
	 */
	Status = XSysMonPsu_SetSeqInputMode(SysMonInstPtr,
			XSYSMONPSU_SEQ_CH1_VAUX00_MASK << XSM_SEQ_CH_SHIFT, XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSysMonPsu_SetSeqAcqTime(SysMonInstPtr,
			(XSYSMONPSU_SEQ_CH1_VAUX0F_MASK | XSYSMONPSU_SEQ_CH1_VAUX00_MASK) << XSM_SEQ_CH_SHIFT,
			XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSysMonPsu_SetSeqInputMode(SysMonInstPtr,
			((u64)XSYSMONPSU_SEQ_CH2_TEMP_RMT_MASK << XSM_SEQ_CH2_SHIFT), XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSysMonPsu_SetSeqAcqTime(SysMonInstPtr,
			((u64)XSYSMONPSU_SEQ_CH2_TEMP_RMT_MASK << XSM_SEQ_CH2_SHIFT),
				XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}



	/*
	 * Enable the averaging on the following channels in the Sequencer
	 * registers:
	 * 	- On-chip Temperature, VCCINT/VCCAUX  supply sensors
	 * 	- 1st/16th Auxiliary Channels
	  *	- Calibration Channel
	 */
	//#define XSYSMONPSU_SEQ_CH2_TEMP_RMT_MASK	0x00000020U
	//#define XSYSMONPSU_TEMP_REMTE_MASK    0x0000ffffU
	//#define XSYSMONPSU_TEMP_MASK    0x0000ffffU
	//#define XSYSMONPSU_CFG_REG1_OVR_TEMP_DIS_MASK    0x00000001U
	Status =  XSysMonPsu_SetSeqAvgEnables(SysMonInstPtr, XSYSMONPSU_SEQ_CH0_TEMP_MASK |
			XSYSMONPSU_SEQ_CH0_SUP1_MASK |
			XSYSMONPSU_SEQ_CH0_SUP3_MASK |
			((XSYSMONPSU_SEQ_CH1_VAUX00_MASK |
			XSYSMONPSU_SEQ_CH1_VAUX0F_MASK) << XSM_SEQ_CH_SHIFT) |
			((u64)XSYSMONPSU_SEQ_CH2_TEMP_RMT_MASK << XSM_SEQ_CH2_SHIFT) |
			XSYSMONPSU_SEQ_CH0_CALIBRTN_MASK, XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the following channels in the Sequencer registers:
	 * 	- On-chip Temperature, VCCINT/VCCAUX supply sensors
	 * 	- 1st/16th Auxiliary Channel
	 *	- Calibration Channel
	 */
	Status =  XSysMonPsu_SetSeqChEnables(SysMonInstPtr, XSYSMONPSU_SEQ_CH0_TEMP_MASK |
			XSYSMONPSU_SEQ_CH0_SUP1_MASK |
			XSYSMONPSU_SEQ_CH0_SUP3_MASK |
			((XSYSMONPSU_SEQ_CH1_VAUX00_MASK |
			XSYSMONPSU_SEQ_CH1_VAUX0F_MASK) << XSM_SEQ_CH_SHIFT) |
			((u64)XSYSMONPSU_SEQ_CH2_TEMP_RMT_MASK << XSM_SEQ_CH2_SHIFT) |
			XSYSMONPSU_SEQ_CH0_CALIBRTN_MASK, XSYSMON_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Clear any bits set in the Interrupt Status Register. */
	IntrStatus = XSysMonPsu_IntrGetStatus(SysMonInstPtr);
	XSysMonPsu_IntrClear(SysMonInstPtr, IntrStatus);

	/* Enable the Channel Sequencer in continuous sequencer cycling mode. */
	XSysMonPsu_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_CONTINPASS, XSYSMON_PS);

	/* Wait till the End of Sequence occurs */
	while ((XSysMonPsu_IntrGetStatus(SysMonInstPtr) & ((u64)XSYSMONPSU_ISR_1_EOS_MASK<< 32)) !=
			((u64)XSYSMONPSU_ISR_1_EOS_MASK<< 32));


	printf("\r\nSysMon PSU Polled Setup was successful. \r\n");
	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function runs a test on the System Monitor device using the
* driver APIs.
* This function does the following tasks:
*	- Initiate the System Monitor device driver instance
*	- Run self-test on the device
*	- Setup the sequence registers to continuously monitor on-chip
*	temperature, VCCINT and VCCAUX
*	- Setup configuration registers to start the sequence
*	- Read the latest on-chip temperature, VCCINT and VCCAUX
*
* @param	SysMonDeviceId is the XPAR_<SYSMON_instance>_DEVICE_ID value
*		from xparameters.h.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note   	None
*
****************************************************************************/
int SysMonPsuPolledPrintfExample(u16 SysMonDeviceId)
{
	u32 TempRawData;
	u32 VccAuxRawData;
	u32 VccIntRawData;
	float TempData;
	float VccAuxData;
	float VccIntData;
	float MaxData;
	float MinData;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;

	//printf("\r\nEntering the SysMon PSU Polled Example. \r\n");
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_TEMP, XSYSMON_PS);
	TempData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);

	printf("\r\nThe Current Temperature is %0d.%03d Centigrades.\r\n",
				(int)(TempData), SysMonPsuFractionToInt(TempData));


	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_TEMP, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	printf("The Maximum Temperature is %0d.%03d Centigrades. \r\n",
				(int)(MaxData), SysMonPsuFractionToInt(MaxData));

	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_TEMP, XSYSMON_PS);
	MinData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	printf("The Minimum Temperature is %0d.%03d Centigrades. \r\n",
				(int)(MinData), SysMonPsuFractionToInt(MinData));

	/*
	 * Read the VccInt Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccIntRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY1, XSYSMON_PS);
	VccIntData = XSysMonPsu_RawToVoltage(VccIntRawData);
	printf("\r\nThe Current VCCINT is %0d.%03d Volts. \r\n",
			(int)(VccIntData), SysMonPsuFractionToInt(VccIntData));

	VccIntRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MAX_SUPPLY1, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToVoltage(VccIntRawData);
	printf("The Maximum VCCINT is %0d.%03d Volts. \r\n",
			(int)(MaxData), SysMonPsuFractionToInt(MaxData));

	VccIntRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MIN_SUPPLY1, XSYSMON_PS);
	MinData = XSysMonPsu_RawToVoltage(VccIntRawData);
	printf("The Minimum VCCINT is %0d.%03d Volts. \r\n",
			(int)(MinData), SysMonPsuFractionToInt(MinData));

	/*
	 * Read the VccAux Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccAuxRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY3, XSYSMON_PS);
	VccAuxData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	printf("\r\nThe Current VCCAUX is %0d.%03d Volts. \r\n",
			(int)(VccAuxData), SysMonPsuFractionToInt(VccAuxData));

	VccAuxRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MAX_SUPPLY3, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	printf("The Maximum VCCAUX is %0d.%03d Volts. \r\n",
				(int)(MaxData), SysMonPsuFractionToInt(MaxData));


	VccAuxRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MIN_SUPPLY3, XSYSMON_PS);
	MinData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	printf("The Minimum VCCAUX is %0d.%03d Volts. \r\n\r\n",
				(int)(MinData), SysMonPsuFractionToInt(MinData));

	//printf("Exiting the SysMon PSU Polled Example. \r\n");

	return XST_SUCCESS;
}


/****************************************************************************/
/**
*
* This function reads current vcc and temp.
*
* @param	SysMonDeviceId is the XPAR_<SYSMON_instance>_DEVICE_ID value
*		from xparameters.h.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note   	None
*
****************************************************************************/
int SysMonPsuPolledReadCurrent(u16 SysMonDeviceId)
{
	u32 TempRawData;
	u32 VccAuxRawData;
	u32 VccIntRawData;
	float TempData;
	float VccAuxData;
	float VccIntData;
	XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	char  *msg = NULL, * msg1 = NULL, * msg2 = NULL, * msg3 = NULL;
	//char* combined_msg = NULL;
	int size = 0;

	//printf("\r\nEntering the SysMon PSU Polled Current. \r\n");
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}

	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_TEMP, XSYSMON_PS);
	TempData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);
	//printf("The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\r\n",
	//			(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);


	asiprintf(&msg,"The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);
	size = strlen(msg);
	//printf("%s", msg);
	sendUart(&UartPs, msg, size);


	TempRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_TEMP_REMTE, XSYSMON_PS);
	TempData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);
	//printf("The Current PSU Remote Temperature is %0d.%03d Centigrades (time: %lu).\r\n",
	//			(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);

	asiprintf(&msg1,"The Current PSU Remote Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);
	size = strlen(msg1);
	//printf("%s ", msg1);
	sendUart(&UartPs, msg1, size);

//	if( psu_rem_temp_max < TempData ){
//			psu_rem_temp_max = TempData;
//			psu_rem_temp_max_changed = 1;
//		}

	/*
	 * Read the VccInt Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccIntRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY1, XSYSMON_PS);
	VccIntData = XSysMonPsu_RawToVoltage(VccIntRawData);
	XTime_GetTime(Xtime);
	//printf("The Current PSU VCCINT is %0d.%03d Volts (time: %lu). \r\n",
	//		(int)(VccIntData), SysMonPsuFractionToInt(VccIntData), *Xtime);

	asiprintf(&msg2,"The Current PSU VCCINT is %0d.%03d Volts (time: %lu).\n", (int)(VccIntData), SysMonPsuFractionToInt(VccIntData), *Xtime);
	size = strlen(msg2);
	//printf("%s", msg2);
	//printf("Send msg size: %i\n", size);
	sendUart(&UartPs, msg2, size);

	/*
	 * Read the VccAux Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccAuxRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY3, XSYSMON_PS);
	VccAuxData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	XTime_GetTime(Xtime);
	//printf("The Current PSU VCCAUX is %0d.%03d Volts (time: %lu). \r\n",
	//		(int)(VccAuxData), SysMonPsuFractionToInt(VccAuxData), *Xtime);

	asiprintf(&msg3,"The Current PSU VCCAUX is %0d.%03d Volts (time: %lu).\n",(int)(VccAuxData), SysMonPsuFractionToInt(VccAuxData), *Xtime);
	size = strlen(msg3);
//	printf("%s", msg3);
	//printf("Send msg size: %i\n", size);
	sendUart(&UartPs, msg3, size);

    // calloc new memory
	//size = strlen(msg) + strlen(msg1)+ strlen(msg2)+ strlen(msg3) + 1;
    //combined_msg = (char *)calloc(size, sizeof(char));
    //strcat(combined_msg, msg);
    //strcat(combined_msg, msg1);
    //strcat(combined_msg, msg2);
    //strcat(combined_msg, msg3);
	//printf("size %i %s",size , combined_msg);
	//sendUart(&UartPs, combined_msg, size);
//	u8 * RecvBuffer = NULL;
//	readUart(&UartPs,RecvBuffer);
//	printf("Data received: %u \n", RecvBuffer);
	//printf("Exiting the SysMon PSU Polled Current. \r\n");
	free(msg);
	free(msg1);
	free(msg2);
	free(msg3);
	//free(combined_msg);
	//printf("Finish sending\n");

	return XST_SUCCESS;
}

int SysMonPsuReadCurrentVccaux(u16 SysMonDeviceId, float * value, XTime * time)
{
	u32 VccAuxRawData;
	//float VccAuxData;
	//XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;

	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the VccAux Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccAuxRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY3, XSYSMON_PS);
	*value = XSysMonPsu_RawToVoltage(VccAuxRawData);
	XTime_GetTime(time);
	//printf("The Current PSU VCCAUX is %0d.%03d Volts (time: %lu). \r\n",
	//		(int)(VccAuxData), SysMonPsuFractionToInt(VccAuxData), *Xtime);

	//asiprintf(msg,"The Current PSU VCCAUX is %0d.%03d Volts (time: %lu).\n",(int)(VccAuxData), SysMonPsuFractionToInt(VccAuxData), *Xtime);
	return XST_SUCCESS;
}

int SysMonPsuReadCurrentVccint(u16 SysMonDeviceId, float * value, XTime * time)
{
	u32 VccIntRawData;
	float VccIntData;
	XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;

	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the VccInt Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccIntRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_SUPPLY1, XSYSMON_PS);
	*value = XSysMonPsu_RawToVoltage(VccIntRawData);
	XTime_GetTime(time);
	//printf("The Current PSU VCCINT is %0d.%03d Volts (time: %lu). \r\n",
	//		(int)(VccIntData), SysMonPsuFractionToInt(VccIntData), *Xtime);

	//asiprintf(msg,"The Current PSU VCCINT is %0d.%03d Volts (time: %lu).\n", (int)(VccIntData), SysMonPsuFractionToInt(VccIntData), *Xtime);
	return XST_SUCCESS;
}

int SysMonPsuReadCurrentTemp(u16 SysMonDeviceId, float * value, XTime * time)
{
	u32 TempRawData;
	//float TempData;
	//XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;

	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_TEMP, XSYSMON_PS);
	*value = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(time);
	//printf("The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\r\n",
	//			(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);


	//printf("%x\n", &msg0);
	//asiprintf(&msg,"The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);
	return XST_SUCCESS;
}

int SysMonPsuReadCurrentTempRem(u16 SysMonDeviceId, float * value, XTime * time)
{
	u32 TempRawData;
	//float TempData;
	//XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;

	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XSysMonPsu_GetAdcData(SysMonInstPtr, XSM_CH_TEMP_REMTE, XSYSMON_PS);
	*value = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(time);
	//printf("The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\r\n",
	//			(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);


	//printf("%x\n", &msg0);
	//asiprintf(&msg,"The Current PSU Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(TempData), SysMonPsuFractionToInt(TempData), *Xtime);
	return XST_SUCCESS;
}


int SysMonPsuReadMinMaxTemp(u16 SysMonDeviceId, float * max, float * min, XTime * time)
{
	u32 rawData;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_TEMP, XSYSMON_PS);
	*max =  XSysMonPsu_RawToTemperature_OnChip(rawData);
	XTime_GetTime(time);
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_TEMP, XSYSMON_PS);
	*min = XSysMonPsu_RawToTemperature_OnChip(rawData);
	return XST_SUCCESS;
}

int SysMonPsuReadMinMaxTempRem(u16 SysMonDeviceId, float * max, float * min, XTime * time)
{
	u32 rawData;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_TEMP_REMOTE, XSYSMON_PS);
	*max =  XSysMonPsu_RawToTemperature_OnChip(rawData);
	XTime_GetTime(time);
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_TEMP_REMOTE, XSYSMON_PS);
	*min = XSysMonPsu_RawToTemperature_OnChip(rawData);
	return XST_SUCCESS;
}

int SysMonPsuReadMinMaxVccint(u16 SysMonDeviceId, float * max, float * min, XTime * time)
{
	u32 rawData;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Voltage Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_SUPPLY1, XSYSMON_PS);
	*max =  XSysMonPsu_RawToVoltage(rawData);
	XTime_GetTime(time);
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_SUPPLY1, XSYSMON_PS);
	*min = XSysMonPsu_RawToVoltage(rawData);
	return XST_SUCCESS;
}

int SysMonPsuReadMinMaxVccaux(u16 SysMonDeviceId, float * max, float * min, XTime * time)
{
	u32 rawData;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Voltage Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_SUPPLY3, XSYSMON_PS);
	*max =  XSysMonPsu_RawToVoltage(rawData);
	XTime_GetTime(time);
	rawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_SUPPLY3, XSYSMON_PS);
	*min = XSysMonPsu_RawToVoltage(rawData);
	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function reads min and max vcc and temp.
*
* @param	SysMonDeviceId is the XPAR_<SYSMON_instance>_DEVICE_ID value
*		from xparameters.h.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note   	None
*
****************************************************************************/
int SysMonPsuPolledReadMinMax(u16 SysMonDeviceId)
{
	u32 TempRawData;
	u32 VccAuxRawData;
	u32 VccIntRawData;
	float MaxData;
	float MinData;
	XTime * Xtime = 0;
	XSysMonPsu *SysMonInstPtr = &SysMonInst;
	char * msg = NULL, * msg1 = NULL, * msg2 = NULL, * msg3 = NULL;
	int size = 0;

	//printf("Entering the SysMon PSU Polled MinMax. \r\n");
	XTime_GetTime(Xtime);
	if(Setup_Done_PSU == 0){
		if( SysMonPsuPolledSetup(SysMonDeviceId, SysMonInstPtr) == XST_SUCCESS){
			Setup_Done_PSU = 1;
		}
	}
	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_TEMP, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);
//	printf("The Maximum PSU Temperature is %0d.%03d Centigrades (time: %lu). \r\n",
//				(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime);

	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_TEMP, XSYSMON_PS);
	MinData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);
//	printf("The Minimum PSU Temperature is %0d.%03d Centigrades (time: %lu). \r\n",
//				(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);

	asiprintf(&msg,"The Maximum PSU Temperature is %0d.%03d Centigrades (time: %lu). \n"
			"The Minimum PSU Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime,
			(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);
	size = strlen(msg);
	//printf("%s", msg);
	sendUart(&UartPs, msg, size);

	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MAX_TEMP_REMOTE, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);

//	printf("The Maximum PSU Remote Temperature is %0d.%03d Centigrades (time: %lu). \r\n",
//				(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime);

	TempRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr, XSM_MIN_TEMP_REMOTE, XSYSMON_PS);
	MinData = XSysMonPsu_RawToTemperature_OnChip(TempRawData);
	XTime_GetTime(Xtime);
//	printf("The Minimum PSU Remote Temperature is %0d.%03d Centigrades (time: %lu). \r\n",
//					(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);

	asiprintf(&msg1,"The Maximum PSU Remote Temperature is %0d.%03d Centigrades (time: %lu). \n"
			"The Minimum PSU Remote Temperature is %0d.%03d Centigrades (time: %lu).\n",(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime,
			(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);
	size = strlen(msg1);
	//printf("%s", msg1);
	sendUart(&UartPs, msg1, size);

	/*
	 * Read the VccInt Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccIntRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MAX_SUPPLY1, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToVoltage(VccIntRawData);
	XTime_GetTime(Xtime);
//	printf("The Maximum PSU VCCINT is %0d.%03d Volts (time: %lu). \r\n",
//			(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime);

	VccIntRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MIN_SUPPLY1, XSYSMON_PS);
	MinData = XSysMonPsu_RawToVoltage(VccIntRawData);
	XTime_GetTime(Xtime);
//	printf("The Minimum PSU VCCINT is %0d.%03d Volts (time: %lu). \r\n",
//			(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);

	asiprintf(&msg2,"The Maximum PSU VCCINT is %0d.%03d Centigrades (time: %lu). \n"
			"The Minimum PSU VCCINT is %0d.%03d Centigrades (time: %lu).\n",(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime,
			(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);
	size = strlen(msg2);
	//printf("%s", msg2);
	sendUart(&UartPs, msg2, size);
	/*
	 * Read the VccAux Votage Data (Current/Maximum/Minimum) from the
	 * ADC data registers.
	 */
	VccAuxRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MAX_SUPPLY3, XSYSMON_PS);
	MaxData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	XTime_GetTime(Xtime);
//	printf("The Maximum PSU VCCAUX is %0d.%03d Volts (time: %lu). \r\n",
//				(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime);


	VccAuxRawData = XSysMonPsu_GetMinMaxMeasurement(SysMonInstPtr,
							XSM_MIN_SUPPLY3, XSYSMON_PS);
	MinData = XSysMonPsu_RawToVoltage(VccAuxRawData);
	XTime_GetTime(Xtime);
//	printf("The Minimum PSU VCCAUX is %0d.%03d Volts (time: %lu). \r\n\r\n",
//				(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);

	asiprintf(&msg3,"The Maximum PSU VCCAUX is %0d.%03d Centigrades (time: %lu). \n"
			"The Minimum PSU VCCAUX is %0d.%03d Centigrades (time: %lu).\n",(int)(MaxData), SysMonPsuFractionToInt(MaxData), *Xtime,
			(int)(MinData), SysMonPsuFractionToInt(MinData), *Xtime);
	size = strlen(msg3);
	//printf("%s", msg3);
	sendUart(&UartPs, msg3, size);

	//printf("Exiting the SysMon PSU Polled MinMax. \r\n");

	free(msg);
	free(msg1);
	free(msg2);
	free(msg3);
	return XST_SUCCESS;
}


/****************************************************************************/
/**
*
* This function converts the fraction part of the given floating point number
* (after the decimal point)to an integer.
*
* @param	FloatNum is the floating point number.
*
* @return	Integer number to a precision of 3 digits.
*
* @note
* This function is used in the printing of floating point data to a STDIO device
* using the xil_printf function. The xil_printf is a very small foot-print
* printf function and does not support the printing of floating point numbers.
*
*****************************************************************************/
int SysMonPsuFractionToInt(float FloatNum)
{
	float Temp;

	Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}

	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
