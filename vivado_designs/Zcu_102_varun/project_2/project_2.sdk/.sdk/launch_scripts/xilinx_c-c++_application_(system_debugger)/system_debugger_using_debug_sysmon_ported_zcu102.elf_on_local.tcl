connect -url tcp:127.0.0.1:3121
source /Software/xilinx/2019.1-sdx/SDK/2019.1/scripts/sdk/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
rst -system
after 3000
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
reset_apu
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C" && level==0} -index 0
fpga -file /home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.runs/impl_1/Base_Zynq_MPSoC_wrapper.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
loadhw -hw /home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/Base_Zynq_MPSoC_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
source /home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/Base_Zynq_MPSoC_wrapper_hw_platform_0/psu_init.tcl
psu_init
after 1000
psu_ps_pl_isolation_removal
after 1000
psu_ps_pl_reset_config
catch {psu_protection}
targets -set -nocase -filter {name =~"*A53*0" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
rst -processor
dow /home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/sysmon_ported_zcu102/Debug/sysmon_ported_zcu102.elf
targets -set -nocase -filter {name =~"*A53*1" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
rst -processor
dow /home/manvar00/Downloads/RELEASE_DVD/vivado_designs/Zcu_102_varun/project_2/project_2.sdk/ZCU102trigger/Debug/ZCU102trigger.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~"*A53*0" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
con
targets -set -nocase -filter {name =~"*A53*1" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A4711C"} -index 1
con
