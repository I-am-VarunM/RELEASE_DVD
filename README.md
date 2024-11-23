## Through Fabric: A Cross-world Thermal Covert Channel on TEE-enhanced FPGA-MPSoC Systems

This repository contains the implementation and evaluation of the paper "**Through Fabric: A Cross-world Thermal Covert Channel on TEE-enhanced FPGA-MPSoC Systems**" accepted at **ASPDAC 2025**.

## Abstract

The ever-evolving computing landscape gets more complex in every moment and the need for heterogeneous compute systems becomes more relevant. As the usability of such systems grew, finding methods for securing them became more relevant. Commercial vendors already introduced Trusted Execution Environments (TEEs) for those systems. TEEs serve the need for isolation, where sensitive data are processed in a secure world, and non-trusted applications are executed in the normal world. In this paper, we introduce Through Fabric: a novel attack against TEE-enhanced FPGA-MPSoCs. We show that existing benign hardware accelerators can be manipulated from the secure world to implement a temperature-based covert channel. We successfully run this attack on a commercial FPGA-MPSoC within the OP-TEE environment without additional access rights. We use an open-source implementation of AES for the accelerator and we reach a transmission speed of 2 bits per second with bit error rate of 1.9% and packet error rate of 4.3%. We are the first to show that a TEE can be bypassed on FPGA-MPSoCs via temperature-based covert channel communication.
Setup and Execution
The implementation is done on a ZCU102 FPGA-MPSoC board, which is supported by the OP-TEE framework.
Prerequisites

1) Vivado 2019.1 or later
2) Xilinx SDK
3) OP-TEE toolchain
4) Python3 with the required packages (numpy, scipy, matplotlib)

## Steps to run

1) Navigate to the Zcu_102_varun directory.
2) Open Vivado and load the project.
3) Make any necessary changes to the block design or the trigger.v file (e.g., modifying the plaintext and key data).
4) Generate the bitstream and export the hardware file, including the bitstream.
5) Launch Xilinx SDK.
6) Adjust the range of data points in the sysmon.c file (in the sysmon_ported_zcu102 folder) and the helloworld.c file.
7) Create a new run configuration in the SDK:

  - Select "Hardware Platform 0" and the exported hardware file.
  - Check the boxes for psu_cortexa53_1, psu_cortexa53_2, sysmon_ported_zcu102, and ZCU102_trigger projects.
  - Select "Program FPGA" on the Target Setup tab.


8) Click "Run" to execute the application.
9) Navigate to the src/data_retrieval directory and run measure.py (python3 measure.py). This will generate the corresponding text files containing the data sent through UART.
10) Run plot.py to generate the plots. Edit the range in the code as needed.

## Notes

1) Sometimes the UART communication might not work. Unplugging and reconnecting the UART cables, as well as power cycling the FPGA board, can help resolve this issue.
2) Avoid adding any print statements other than the asiprintf calls in sysmon.c, as they can corrupt the data being sent through UART.
3) Check the linker script in the application projects to ensure the ddr_0_MEM0 address does not overlap.
