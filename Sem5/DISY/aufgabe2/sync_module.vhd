
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;

ENTITY sync_module IS
	GENERIC (RSTDEF : STD_LOGIC := '1');
	PORT (
		rst : IN STD_LOGIC; -- reset, active RSTDEF
		clk : IN STD_LOGIC; -- clock, risign edge
		swrst : IN STD_LOGIC; -- software reset, active RSTDEF
		BTN1 : IN STD_LOGIC; -- push button -> load
		BTN2 : IN STD_LOGIC; -- push button -> dec
		BTN3 : IN STD_LOGIC; -- push button -> inc
		load : OUT STD_LOGIC; -- load,      high active
		dec : OUT STD_LOGIC; -- decrement, high active
		inc : OUT STD_LOGIC); -- increment, high active
END sync_module;

--
-- Im Rahmen der 2. Aufgabe soll hier die Architekturbeschreibung
-- zur Entity sync_module implementiert werden.
--

ARCHITECTURE structure OF sync_module IS
	COMPONENT sync_buffer IS
		GENERIC (RSTDEF : STD_LOGIC := '1');
		PORT (
			rst : IN STD_LOGIC; -- reset, RSTDEF active
			clk : IN STD_LOGIC; -- clock, rising edge
			en : IN STD_LOGIC; -- enable, high active
			swrst : IN STD_LOGIC; -- software reset, RSTDEF active
			din : IN STD_LOGIC; -- data bit, input
			dout : OUT STD_LOGIC; -- data bit, output
			redge : OUT STD_LOGIC; -- rising  edge on din detected
			fedge : OUT STD_LOGIC); -- falling edge on din detected
	END COMPONENT;

	-- Modulo 2**15 Zähler
	CONSTANT N15 : NATURAL := 2 ** 15;
	SIGNAL cnt15 : INTEGER RANGE 0 TO N15 - 1;
	SIGNAL strb15 : STD_LOGIC; -- strobe for mod-15-zaehler, active high

BEGIN
	-- Modulo-2**15-Zaehler -> Frequenzteiler
	p1 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			cnt15 <= 0;
			strb15 <= '0';
		ELSIF rising_edge(clk) THEN
			IF swrst = RSTDEF THEN
				cnt15 <= 0;
				strb15 <= '0';
			END IF;
			IF cnt15 = N15 - 1 THEN
				cnt15 <= 0;
				strb15 <= '1'; -- für die sync_buffer
			ELSE
				cnt15 <= cnt15 + 1;
				strb15 <= '0';
			END IF;
		END IF;
	END PROCESS;

	buf1 : sync_buffer
	GENERIC MAP(RSTDEF => RSTDEF)
	PORT MAP(
		rst => rst,
		clk => clk,
		en => strb15,
		swrst => swrst,
		din => BTN1,
		dout => OPEN, --inc,
		redge => OPEN,
		fedge => inc); --OPEN);

	buf2 : sync_buffer
	GENERIC MAP(RSTDEF => RSTDEF)
	PORT MAP(
		rst => rst,
		clk => clk,
		en => strb15,
		swrst => swrst,
		din => BTN2,
		dout => OPEN, --dec,
		redge => OPEN,
		fedge => dec); --OPEN);

	buf3 : sync_buffer
	GENERIC MAP(RSTDEF => RSTDEF)
	PORT MAP(
		rst => rst,
		clk => clk,
		en => strb15,
		swrst => swrst,
		din => BTN3,
		dout => OPEN, --load,
		redge => load, --OPEN,
		fedge => OPEN);

END structure;