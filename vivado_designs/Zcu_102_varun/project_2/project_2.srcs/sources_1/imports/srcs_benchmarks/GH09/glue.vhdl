library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity B1 is

  port (
    iClkFast : in std_logic;
    iClkSlow : in std_logic;
    iReset   : in std_logic;

    iCommandBus : in std_logic_vector(7 downto 0);
    iIdle       : in std_logic;

    oCount12 : out std_logic_vector(11 downto 0);
    oCount8  : out std_logic_vector(7 downto 0);
    oCount4  : out std_logic_vector(3 downto 0)
    );
end B1;

architecture RTL of B1 is

  type tState is (Reset, Idle, Waiting, ProcessFast, ProcessSlow);
  signal sState12, sNextState12 : tState;
  signal sState8, sNextState8   : tState;
  signal sState4, sNextState4   : tState;

  signal sCount12Slow : std_logic_vector(11 downto 0) := (others => '0');
  signal sCount8Slow  : std_logic_vector(7 downto 0)  := (others => '0');
  signal sCount4Slow  : std_logic_vector(3 downto 0)  := (others => '0');

  signal sCount12Fast : std_logic_vector(11 downto 0) := (others => '0');
  signal sCount8Fast  : std_logic_vector(7 downto 0)  := (others => '0');
  signal sCount4Fast  : std_logic_vector(3 downto 0)  := (others => '0');

  signal sEnableCount12Fast, sEnableCount12Slow, sLoadCount12Fast, sLoadCount12Fast_D, sLoadCount12Slow : std_logic;
  signal sEnableCount8Fast, sEnableCount8Slow, sLoadCount8Fast, sLoadCount8Fast_D, sLoadCount8Slow      : std_logic;
  signal sEnableCount4Fast, sEnableCount4Slow, sLoadCount4Fast, sLoadCount4Fast_D, sLoadCount4Slow      : std_logic;

begin  -- RTL

  output12 : process (sCount12Slow, sCount12Fast, sEnableCount12Fast)
  begin
    oCount12   <= sCount12Slow;
    if(sEnableCount12Fast = '1') then
      oCount12 <= sCount12Fast;
    end if;
  end process;

  output8 : process (sCount8Slow, sCount8Fast, sEnableCount8Fast)
  begin
    oCount8   <= sCount8Slow;
    if(sEnableCount8Fast = '1') then
      oCount8 <= sCount8Fast;
    end if;
  end process;

  output4 : process (sCount4Slow, sCount4Fast, sEnableCount4Fast)
  begin
    oCount4   <= sCount4Slow;
    if(sEnableCount4Fast = '1') then
      oCount4 <= sCount4Fast;
    end if;
  end process;


  Fast12 : process (iClkFast, iReset)
  begin
    if(iReset = '1') then
      sCount12Fast   <= (others => '0');
    elsif (iClkFast'event and iClkFast = '1') then
      if(sLoadCount12Fast = '1' and sLoadCount12Fast_D = '0') then
        sCount12Fast <= sCount12Slow;
      elsif (sEnableCount12Fast = '1') then
        sCount12Fast <= sCount12Fast + 1;
      end if;
    end if;
  end process Fast12;

  Slow12 : process (iClkSlow, iReset)
  begin
    if(iReset = '1') then
      sCount12Slow   <= (others => '0');
    elsif (iClkSlow'event and iClkSlow = '1') then
      if(sLoadCount12Slow = '1') then
        sCount12Slow <= sCount12Fast;
      elsif (sEnableCount12Slow = '1') then
        sCount12Slow <= sCount12Slow + 1;
      end if;
    end if;
  end process Slow12;


  Fast8 : process (iClkFast, iReset)
  begin
    if(iReset = '1') then
      sCount8Fast   <= (others => '0');
    elsif (iClkFast'event and iClkFast = '1') then
      if(sLoadCount8Fast = '1' and sLoadCount8Fast_D = '0') then
        sCount8Fast <= sCount8Slow;
      elsif (sEnableCount8Fast = '1') then
        sCount8Fast <= sCount8Fast + 1;
      end if;
    end if;
  end process Fast8;

  Slow8 : process (iClkSlow, iReset)
  begin
    if(iReset = '1') then
      sCount8Slow   <= (others => '0');
    elsif (iClkSlow'event and iClkSlow = '1') then
      if(sLoadCount8Slow = '1') then
        sCount8Slow <= sCount8Fast;
      elsif (sEnableCount8Slow = '1') then
        sCount8Slow <= sCount8Slow + 1;
      end if;
    end if;
  end process Slow8;

  Fast4 : process (iClkFast, iReset)
  begin
    if(iReset = '1') then
      sCount4Fast   <= (others => '0');
    elsif (iClkFast'event and iClkFast = '1') then
      if(sLoadCount4Fast = '1' and sLoadCount4Fast_D = '0') then
        sCount4Fast <= sCount4Slow;
      elsif (sEnableCount4Fast = '1') then
        sCount4Fast <= sCount4Fast + 1;
      end if;
    end if;
  end process Fast4;

  Slow4 : process (iClkSlow, iReset)
  begin
    if(iReset = '1') then
      sCount4Slow   <= (others => '0');
    elsif (iClkSlow'event and iClkSlow = '1') then
      if(sLoadCount4Slow = '1') then
        sCount4Slow <= sCount4Fast;
      elsif (sEnableCount4Slow = '1') then
        sCount4Slow <= sCount4Slow + 1;
      end if;
    end if;
  end process Slow4;


  FSM_regs : process (iClkSlow, iReset)
  begin
    if iReset = '1' then
      sState12 <= Reset;
      sState8  <= Reset;
      sState4  <= Reset;

      sLoadCount12Fast_D <= '0';
      sLoadCount8Fast_D  <= '0';
      sLoadCount4Fast_D  <= '0';
    elsif iClkSlow'event and iClkSlow = '1' then
      sState12           <= sNextState12;
      sState8            <= sNextState8;
      sState4            <= sNextState4;

      sLoadCount12Fast_D <= sLoadCount12Fast;
      sLoadCount8Fast_D  <= sLoadCount8Fast;
      sLoadCount4Fast_D  <= sLoadCount4Fast;
    end if;
  end process FSM_regs;

  FSM12 : process (iCommandBus, iIdle, sState12)
  begin
    sLoadCount12Slow <= '0';
    sLoadCount12Fast <= '0';

    sEnableCount12Fast <= '0';
    sEnableCount12Slow <= '0';

    case sState12 is
      when Reset =>
        sNextState12   <= Reset;
        if (iReset = '0') then
          sNextState12 <= Idle;
        end if;

      when Idle =>
        sNextState12   <= Idle;
        if(iIdle = '0') then
          sNextState12 <= Waiting;
        end if;

      when Waiting =>
        sNextState12       <= Waiting;
        if(iIdle = '1') then
          sNextState12     <= Idle;
        elsif(iCommandBus = "00000010") then
          sLoadCount12Fast <= '1';
          sNextState12     <= ProcessFast;
        elsif(iCommandBus = "00000011") then
          sLoadCount12Slow <= '1';
          sNextState12     <= ProcessSlow;
        end if;

      when ProcessFast =>
        sEnableCount12Fast <= '1';
        sNextState12       <= ProcessFast;
        if(iCommandBus = "00000001") then
          sNextState12     <= Waiting;
        elsif(iCommandBus = "00000011") then
          sLoadCount12Slow <= '1';
          sNextState12     <= ProcessSlow;
        end if;

      when ProcessSlow =>
        sEnableCount12Slow <= '1';
        sNextState12       <= ProcessSlow;
        if(iCommandBus = "00000001") then
          sNextState12     <= Waiting;
        elsif(iCommandBus = "00000010") then
          sLoadCount12Fast <= '1';
          sNextState12     <= ProcessFast;
        end if;

      when others => null;
    end case;
  end process FSM12;

  FSM8 : process (iCommandBus, iIdle, sState8)
  begin
    sLoadCount8Slow <= '0';
    sLoadCount8Fast <= '0';

    sEnableCount8Fast <= '0';
    sEnableCount8Slow <= '0';

    case sState8 is
      when Reset =>
        sNextState8   <= Reset;
        if (iReset = '0') then
          sNextState8 <= Idle;
        end if;

      when Idle =>
        sNextState8   <= Idle;
        if(iIdle = '0') then
          sNextState8 <= Waiting;
        end if;

      when Waiting =>
        sNextState8       <= Waiting;
        if(iIdle = '1') then
          sNextState8     <= Idle;
        elsif(iCommandBus = "00000110") then
          sLoadCount8Fast <= '1';
          sNextState8     <= ProcessFast;
        elsif(iCommandBus = "00000111") then
          sLoadCount8Slow <= '1';
          sNextState8     <= ProcessSlow;
        end if;

      when ProcessFast =>
        sEnableCount8Fast <= '1';
        sNextState8       <= ProcessFast;
        if(iCommandBus = "00000101") then
          sNextState8     <= Waiting;
        elsif(iCommandBus = "00000111") then
          sLoadCount8Slow <= '1';
          sNextState8     <= ProcessSlow;
        end if;

      when ProcessSlow =>
        sEnableCount8Slow <= '1';
        sNextState8       <= ProcessSlow;
        if(iCommandBus = "00000101") then
          sNextState8     <= Waiting;
        elsif(iCommandBus = "00000110") then
          sLoadCount8Fast <= '1';
          sNextState8     <= ProcessFast;
        end if;

      when others => null;
    end case;
  end process FSM8;

  FSM4 : process (iCommandBus, iIdle, sState4)
  begin
    sLoadCount4Slow <= '0';
    sLoadCount4Fast <= '0';

    sEnableCount4Fast <= '0';
    sEnableCount4Slow <= '0';

    case sState4 is
      when Reset =>
        sNextState4   <= Reset;
        if (iReset = '0') then
          sNextState4 <= Idle;
        end if;

      when Idle =>
        sNextState4   <= Idle;
        if(iIdle = '0') then
          sNextState4 <= Waiting;
        end if;

      when Waiting =>
        sNextState4       <= Waiting;
        if(iIdle = '1') then
          sNextState4     <= Idle;
        elsif(iCommandBus = "00001110") then
          sLoadCount4Fast <= '1';
          sNextState4     <= ProcessFast;
        elsif(iCommandBus = "00001111") then
          sLoadCount4Slow <= '1';
          sNextState4     <= ProcessSlow;
        end if;

      when ProcessFast =>
        sEnableCount4Fast <= '1';
        sNextState4       <= ProcessFast;
        if(iCommandBus = "00001101") then
          sNextState4     <= Waiting;
        elsif(iCommandBus = "00001111") then
          sLoadCount4Slow <= '1';
          sNextState4     <= ProcessSlow;
        end if;

      when ProcessSlow =>
        sEnableCount4Slow <= '1';
        sNextState4       <= ProcessSlow;
        if(iCommandBus = "00001101") then
          sNextState4     <= Waiting;
        elsif(iCommandBus = "00001110") then
          sLoadCount4Fast <= '1';
          sNextState4     <= ProcessFast;
        end if;

      when others => null;
    end case;
  end process FSM4;


end RTL;
