onbreak {quit -force}
onerror {quit -force}

asim -t 1ps +access +r +m+Base_Zynq_MPSoC -L xilinx_vip -L xil_defaultlib -L xpm -L axi_infrastructure_v1_1_0 -L axi_vip_v1_1_5 -L zynq_ultra_ps_e_vip_v1_0_5 -L axi_lite_ipif_v3_0_4 -L lib_cdc_v1_0_2 -L interrupt_control_v3_1_4 -L axi_gpio_v2_0_21 -L proc_sys_reset_v5_0_13 -L generic_baseblocks_v2_1_0 -L axi_register_slice_v2_1_19 -L fifo_generator_v13_2_4 -L axi_data_fifo_v2_1_18 -L axi_crossbar_v2_1_20 -L axi_protocol_converter_v2_1_19 -L axi_clock_converter_v2_1_18 -L blk_mem_gen_v8_4_3 -L axi_dwidth_converter_v2_1_19 -L axi_mmu_v2_1_17 -L xlconcat_v2_1_3 -L xlslice_v1_0_2 -L xilinx_vip -L unisims_ver -L unimacro_ver -L secureip -O5 xil_defaultlib.Base_Zynq_MPSoC xil_defaultlib.glbl

do {wave.do}

view wave
view structure

do {Base_Zynq_MPSoC.udo}

run -all

endsim

quit -force
