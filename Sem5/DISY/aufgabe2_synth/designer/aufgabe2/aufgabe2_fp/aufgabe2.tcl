open_project -project {C:\Users\ds-02\Documents\DISY\aufgabe2_synth\designer\aufgabe2\aufgabe2_fp\aufgabe2.pro}
enable_device -name {M2S005} -enable 1
set_programming_file -name {M2S005} -file {C:\Users\ds-02\Documents\DISY\aufgabe2_synth\designer\aufgabe2\aufgabe2.ppd}
set_programming_action -action {PROGRAM} -name {M2S005} 
run_selected_actions
save_project
close_project
