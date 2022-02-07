new_project \
         -name {aufgabe1} \
         -location {C:\Users\ds-02\Documents\DISY\aufgabe1_synth\designer\aufgabe1\aufgabe1_fp} \
         -mode {chain} \
         -connect_programmers {FALSE}
add_actel_device \
         -device {M2S005} \
         -name {M2S005}
enable_device \
         -name {M2S005} \
         -enable {TRUE}
save_project
close_project
