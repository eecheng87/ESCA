reset
set ylabel 'time(ns) / request'
set xlabel 'client'
set title 'dBatch vs normal Mongoose web server'
set key left top
set term png enhanced font 'Verdana,10'
set output 'exp2.png'

plot [:][:] \
'db.out' using ($0+1):1 with linespoints linewidth 2 title "dBatch server",\
'nor.out' using ($0+1):1 with linespoints linewidth 2 title "normal server"