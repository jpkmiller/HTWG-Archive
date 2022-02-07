set_device -family {SmartFusion2} -die {M2S005} -speed {STD}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe1_synth\hdl\hex4x7seg.vhd}
read_vhdl -mode vhdl_2008 {C:\Users\ds-02\Documents\DISY\aufgabe1_synth\hdl\aufgabe1.vhd}
set_top_level {aufgabe1}
map_netlist
check_constraints {C:\Users\ds-02\Documents\DISY\aufgabe1_synth\constraint\synthesis_sdc_errors.log}
write_fdc {C:\Users\ds-02\Documents\DISY\aufgabe1_synth\designer\aufgabe1\synthesis.fdc}
