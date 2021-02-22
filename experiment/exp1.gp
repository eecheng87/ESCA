reset
set ylabel 'time(ns)'
set title 'dBatch vs batch vs normal consecutive write'
set key left top
set term png enhanced font 'Verdana,10'
set output 'exp1.png'

plot [:][:] \
'n3.out' using ($0+1):1 with linespoints linewidth 2 title "normal write",\
'db3.out' using ($0+1):1 with linespoints linewidth 2 title "dBatch write", \
'b3.out' using ($0+1):1 with linespoints linewidth 2 title "batch write"