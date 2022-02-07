set_device -family {SmartFusion2} -die {M2S005} -speed {STD}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe2\std_counter.vhd}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe2\sync_buffer.vhd}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe2\sync_module.vhd}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe2\hex4x7seg.vhd}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe2\aufgabe2.vhd}
set_top_level {aufgabe2}
map_netlist
check_constraints {C:\Users\ds-02\Documents\DISY\aufgabe2_synth\constraint\synthesis_sdc_errors.log}
write_fdc {C:\Users\ds-02\Documents\DISY\aufgabe2_synth\designer\aufgabe2\synthesis.fdc}
