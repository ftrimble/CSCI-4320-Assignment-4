set terminal post eps
set xlabel 'Number of Threads per Node'
set output 'plots/time.eps'
set ylabel 'Execution Time (seconds)'
set title 'Execution Time vs. Number of Threads per Node'
plot 'data/time.dat' w l
