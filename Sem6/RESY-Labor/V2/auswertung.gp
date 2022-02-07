set title "{/=20 Sleep times with I/O load with priority}"
set terminal postscript eps color colortext
set xlabel "{/=20Actual period to sleep [us]}\n"
set ylabel "{/=20Measured sleep time [us]}"
set term eps
set output filename . '.eps'
set grid
set autoscale
set style data lines
show style data
plot filename using ($1):($2) title "{/=17 best}" lt 10,\
     filename using ($1):($3) title "{/=17 mean}" lt 9, \
     filename using ($1):($4) title "{/=17 worst}" lt 7
set output