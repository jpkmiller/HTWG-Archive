
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;

ENTITY sync_buffer IS
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
END sync_buffer;

--
-- Im Rahmen der 2. Aufgabe soll hier die Architekturbeschreibung
-- zur Entity sync_buffer implementiert werden.
--

ARCHITECTURE verhalten OF sync_buffer IS

	CONSTANT SAMPLES : NATURAL := 31;
	SIGNAL cnt : INTEGER RANGE 0 TO SAMPLES;
	SIGNAL state : STD_LOGIC;
	SIGNAL dout_t : STD_LOGIC;
	SIGNAL dff1, dff2, dff3 : STD_LOGIC;
	SIGNAL r1 : STD_LOGIC; -- help detecing rising/falling edge
	SIGNAL r2 : STD_LOGIC; -- help detecing rising/falling edge

BEGIN

	p1 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			dff1 <= '0'; --asynchron
		ELSIF rising_edge(clk) THEN
			dff1 <= din; --synchron
		END IF;
	END PROCESS;


	-- dff2 hat Einfluss auf FlipFlop -> Hysterese anpassen und umschreiben
	p2 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			dff2 <= '0'; --asynchron
		ELSIF rising_edge(clk) THEN
			dff2 <= dff1; --synchron
		END IF;
	END PROCESS;


	p3 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			cnt <= 0;
			state <= '0';
			dout_t <= '0';
		ELSE
			IF rising_edge(clk) THEN
				IF swrst = RSTDEF THEN
					cnt <= 0;
					state <= '0';
					dout_t <= '0';
				ELSE
					IF en = '1' THEN  -- muss dabei sein!

						CASE state IS
							WHEN '0' =>
								dout_t <= '0';
								IF dff2 = '0' THEN
									IF cnt > 0 THEN
										cnt <= cnt - 1;
									END IF;

								ELSE -- dff2 = '1'

									IF cnt < SAMPLES THEN
										cnt <= cnt + 1;
									ELSE
										state <= '1';
									END IF;
								END IF;

							WHEN '1' =>
								dout_t <= '1';
								IF dff2 = '1' THEN
									IF cnt < SAMPLES THEN
										cnt <= cnt + 1;
									END IF;

								ELSE -- dff2 = '0'

									IF cnt > 0 THEN
										cnt <= cnt - 1;
									ELSE
										state <= '0';
									END IF;
								END IF;
						END CASE;
					END IF;
				END IF;
			END IF;
		END IF;
	END PROCESS;


	dout <= dff3;
	redge <= NOT dff3 AND dout_t;
	fedge <= NOT dout_t AND dff3;


	p4 : PROCESS (rst, clk) IS
	BEGIN
		IF rst = RSTDEF THEN
			dff3 <= '0'; --asynchron
		ELSIF rising_edge(clk) THEN
			dff3 <= dout_t; --synchron
		END IF;
	END PROCESS;

END verhalten;