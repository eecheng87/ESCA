
set ylabel 'time(ns)'
set title 'batch vs normal server(multi-thread)'
set term png enhanced font 'Verdana,10'
set style data boxplot
set style boxplot outliers

unset key
set xtics ("normal server" 1, "batch server" 2) scale 0.0

set output 'exp2.png'
set yrange [0:255000]

plot \
'normal.out' using (1):1 ,\
'batch.out' using (2):1 ,\