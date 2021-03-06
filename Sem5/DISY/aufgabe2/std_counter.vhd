
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;
USE ieee.numeric_std.ALL;

ENTITY std_counter IS
   GENERIC (
      RSTDEF : STD_LOGIC := '1';
      CNTLEN : NATURAL := 4);
   PORT (
      rst : IN STD_LOGIC; -- reset,           RSTDEF active
      clk : IN STD_LOGIC; -- clock,           rising edge
      en : IN STD_LOGIC; -- enable,          high active
      inc : IN STD_LOGIC; -- increment,       high active
      dec : IN STD_LOGIC; -- decrement,       high active
      load : IN STD_LOGIC; -- load value,      high active
      swrst : IN STD_LOGIC; -- software reset,  RSTDEF active
      cout : OUT STD_LOGIC; -- carry,           high active
      din : IN STD_LOGIC_VECTOR(CNTLEN - 1 DOWNTO 0); -- von den switches (von universalzähler...
      dout : OUT STD_LOGIC_VECTOR(CNTLEN - 1 DOWNTO 0)); -- ...zu unserer Segmentanzeige)
END std_counter;

--
-- Funktionstabelle
-- rst clk swrst en  load dec inc | Aktion
----------------------------------+-------------------------
--  V   -    -    -    -   -   -  | cnt := 000..0, asynchrones Reset
--  N   r    V    -    -   -   -  | cnt := 000..0, synchrones  Reset
--  N   r    N    0    -   -   -  | keine Aenderung
--  N   r    N    1    1   -   -  | cnt := din, paralleles Laden
--  N   r    N    1    0   1   -  | cnt := cnt - 1, dekrementieren
--  N   r    N    1    0   0   1  | cnt := cnt + 1, inkrementieren
--  N   r    N    1    0   0   0  | keine Aenderung
--
-- Legende:
-- V = valid, = RSTDEF
-- N = not valid, = NOT RSTDEF
-- r = rising egde
-- din = Dateneingang des Zaehlers
-- cnt = Wert des Zaehlers
--

--
-- Im Rahmen der 2. Aufgabe soll hier die Architekturbeschreibung
-- zur Entity std_counter implementiert werden
--

ARCHITECTURE verhalten OF std_counter IS

   SIGNAL cnt : STD_LOGIC_VECTOR(CNTLEN - 1 DOWNTO 0);
   SIGNAL zeros : STD_LOGIC_VECTOR(CNTLEN - 1 DOWNTO 0) := (OTHERS => '0');
   SIGNAL ones : STD_LOGIC_VECTOR(CNTLEN - 1 DOWNTO 0) := (OTHERS => '1');

BEGIN

   p1 : PROCESS (rst, clk) IS
   BEGIN
      IF rst = RSTDEF THEN
         cout <= '0';
         cnt <= (OTHERS => '0');
      ELSIF rising_edge(clk) THEN
         IF swrst = RSTDEF THEN
            cout <= '0';
            cnt <= (OTHERS => '0');
         ELSIF en = '1' THEN
            IF load = '1' THEN
               cout <= '0';
               cnt <= din;
            ELSIF dec = '1' THEN
               -- carry
               IF cnt = zeros THEN
                  cout <= '1';
               ELSE
                  cout <= '0';
               END IF;
               cnt <= cnt - '1';
            ELSIF inc = '1' THEN
               -- carry
               IF cnt = ones THEN
                  cout <= '1';
               ELSE
                  cout <= '0';
               END IF;
               cnt <= cnt + '1';
            END IF;
         END IF;
      END IF;
   END PROCESS;

   dout <= cnt;

END verhalten;