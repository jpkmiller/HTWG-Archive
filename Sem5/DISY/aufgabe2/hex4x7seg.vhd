
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;

ENTITY hex4x7seg IS
	GENERIC (RSTDEF : STD_LOGIC := '0');
	PORT (
		rst : IN STD_LOGIC; -- reset, active RSTDEF
		clk : IN STD_LOGIC; -- clock, rising edge
		data : IN STD_LOGIC_VECTOR(15 DOWNTO 0); -- data input, active high
		dpin : IN STD_LOGIC_VECTOR(3 DOWNTO 0); -- 4 decimal point, active high
		ena : OUT STD_LOGIC_VECTOR(3 DOWNTO 0); -- 4 digit enable  signals, active high
		seg : OUT STD_LOGIC_VECTOR(7 DOWNTO 1); -- 7 connections to seven-segment display, active high
		dp : OUT STD_LOGIC); -- decimal point output, active high

END hex4x7seg;

ARCHITECTURE struktur OF hex4x7seg IS

-- Modulo-2**14-Zaehler
CONSTANT N : NATURAL := 2 ** 14;
SIGNAL cntMod : INTEGER RANGE 0 TO N - 1;
SIGNAL strb : STD_LOGIC; -- strobe for mod-4-zaehler, active high

SIGNAL ena_t : STD_LOGIC_VECTOR(3 DOWNTO 0);

-- Modulo-4-Zaehler
CONSTANT N4 : NATURAL := 4;
SIGNAL cnt4 : INTEGER RANGE 0 TO N4 - 1;

-- sw for SW8 ... SW1
SIGNAL sw_out : STD_LOGIC_VECTOR(3 DOWNTO 0);

BEGIN

	-- Modulo-2**14-Zaehler -> Frequenzteiler
	p1 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			cntMod <= 0;
			strb <= '0';
		ELSIF rising_edge(clk) THEN
			IF cntMod = N - 1 THEN
				cntMod <= 0;
				strb <= '1'; -- set signal strb14 to control mod-4-zaehler
			ELSE
				cntMod <= cntMod + 1;
				strb <= '0';
			END IF;
		END IF;
	END PROCESS;

	-- Modulo-4-Zaehler
	p2 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			cnt4 <= 0;
		ELSIF rising_edge(clk) THEN
			IF strb = '1' THEN
				IF cnt4 = N4 - 1 THEN
					cnt4 <= 0;
				ELSE
					cnt4 <= cnt4 + 1;
				END IF;
			END IF;
		END IF;
	END PROCESS;

	-- 1-aus-4-Dekoder als Phasengenerator
	WITH cnt4 SELECT
		ena_t <= "0001" WHEN 0,
		"0010" WHEN 1,
		"0100" WHEN 2,
		"1000" WHEN 3;
	ena <= ena_t WHEN rst /= RSTDEF ELSE "0000";

	-- 1-aus-4-Multiplexer für die Dezimalpunkte
	WITH cnt4 SELECT
		dp <= dpin(0) WHEN 0,
		dpin(1) WHEN 1,
		dpin(2) WHEN 2,
		dpin(3) WHEN 3;

	-- 1-aus-4-Multiplexer in zwei 4-Bit-Blöcken für die Schiebeschalter
	WITH cnt4 SELECT
		sw_out <= data(3 DOWNTO 0) WHEN 0, -- DIGIT1 WHEN 0
		data(7 DOWNTO 4) WHEN 1, -- DIGIT2 WHEN 1
		data(11 DOWNTO 8) WHEN 2, -- DIGIT3 WHEN 2
		data(15 DOWNTO 12) WHEN 3; -- DIGIT4 WHEN 3

	-- 7-aus-4-Dekoder
	WITH sw_out SELECT
		seg <= "0111111" WHEN "0000", -- 0
		"0000110" WHEN "0001", -- 1
		"1011011" WHEN "0010", -- 2
		"1001111" WHEN "0011", -- 3
		"1100110" WHEN "0100", -- 4
		"1101101" WHEN "0101", -- 5
		"1111101" WHEN "0110", -- 6
		"0000111" WHEN "0111", -- 7
		"1111111" WHEN "1000", -- 8
		"1101111" WHEN "1001", -- 9
		"1110111" WHEN "1010", -- a
		"1111100" WHEN "1011", -- b
		"0111001" WHEN "1100", -- c
		"1011110" WHEN "1101", -- d
		"1111001" WHEN "1110", -- e
		"1110001" WHEN "1111", -- f
		"0000000" WHEN OTHERS;

	END struktur;