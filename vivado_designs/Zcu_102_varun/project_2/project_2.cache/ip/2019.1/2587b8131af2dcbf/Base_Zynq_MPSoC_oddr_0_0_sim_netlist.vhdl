-- Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2019.1 (win64) Build 2552052 Fri May 24 14:49:42 MDT 2019
-- Date        : Mon Sep 23 18:42:18 2019
-- Host        : DESKTOP-G3QMJB5 running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ Base_Zynq_MPSoC_oddr_0_0_sim_netlist.vhdl
-- Design      : Base_Zynq_MPSoC_oddr_0_0
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xczu7ev-ffvc1156-2-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_oddr_v1_0_0_oddr is
  port (
    clk_out : out STD_LOGIC;
    clk_in : in STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_oddr_v1_0_0_oddr;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_oddr_v1_0_0_oddr is
  signal NLW_ODDRE1_inst_CLKDIV_UNCONNECTED : STD_LOGIC;
  signal NLW_ODDRE1_inst_T_OUT_UNCONNECTED : STD_LOGIC;
  signal NLW_ODDRE1_inst_D_UNCONNECTED : STD_LOGIC_VECTOR ( 7 downto 1 );
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ODDRE1_inst : label is "PRIMITIVE";
  attribute XILINX_LEGACY_PRIM : string;
  attribute XILINX_LEGACY_PRIM of ODDRE1_inst : label is "ODDRE1";
  attribute XILINX_TRANSFORM_PINMAP : string;
  attribute XILINX_TRANSFORM_PINMAP of ODDRE1_inst : label is "C:CLK SR:RST Q:OQ D1:D[0] D2:D[4]";
begin
ODDRE1_inst: unisim.vcomponents.OSERDESE3
    generic map(
      INIT => '0',
      IS_CLK_INVERTED => '0',
      ODDR_MODE => "TRUE",
      OSERDES_T_BYPASS => "TRUE",
      SIM_DEVICE => "ULTRASCALE_PLUS"
    )
        port map (
      CLK => clk_in,
      CLKDIV => NLW_ODDRE1_inst_CLKDIV_UNCONNECTED,
      D(7 downto 5) => NLW_ODDRE1_inst_D_UNCONNECTED(7 downto 5),
      D(4) => '0',
      D(3 downto 1) => NLW_ODDRE1_inst_D_UNCONNECTED(3 downto 1),
      D(0) => '1',
      OQ => clk_out,
      RST => '0',
      T => '0',
      T_OUT => NLW_ODDRE1_inst_T_OUT_UNCONNECTED
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  port (
    clk_in : in STD_LOGIC;
    clk_out : out STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "Base_Zynq_MPSoC_oddr_0_0,oddr_v1_0_0_oddr,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "yes";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "oddr_v1_0_0_oddr,Vivado 2019.1";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of clk_in : signal is "xilinx.com:signal:clock:1.0 clk_in CLK";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of clk_in : signal is "XIL_INTERFACENAME clk_in, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN Base_Zynq_MPSoC_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of clk_out : signal is "xilinx.com:signal:clock:1.0 clk_out CLK";
  attribute X_INTERFACE_PARAMETER of clk_out : signal is "XIL_INTERFACENAME clk_out, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN Base_Zynq_MPSoC_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0";
begin
inst: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_oddr_v1_0_0_oddr
     port map (
      clk_in => clk_in,
      clk_out => clk_out
    );
end STRUCTURE;
