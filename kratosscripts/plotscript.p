set terminal post eps
set xlabel 'Number of Threads per Node'
set logscale y 2
set logscale x 2

################################
##### Execution time plots #####
################################

set output 'plots/exec.eps'
set ylabel 'Execution Time (seconds)'
set title 'Execution Time vs. Number of Threads Per Node'
plot 'data/execAvg.dat' w l title 'Average', \
    'data/execMin.dat' w l title 'Minimum',  \
    'data/execMax.dat' w l title 'Maximum'

###################################################
##### Effective communication bandwidth plots #####
###################################################

set output 'plots/comm.eps'
set ylabel 'Communication Bandwidth (bytes/second)'
set title 'Communication Bandwidth vs. Number of Threads Per Node'
plot 'data/commAvg.dat' w l title 'Average', \
    'data/commMin.dat' w l title 'Minimum',  \
    'data/commMax.dat' w l title 'Maximum'

######################################
##### Matrix Multiply time plots #####
######################################

set output 'plots/mm.eps'
set ylabel 'Matrix Multiply Time (seconds)'
set title 'Matrix Multiply Time vs. Number of Threads Per Node'
plot 'data/mmAvg.dat' w l title 'Average', \
    'data/mmMin.dat' w l title 'Minimum',  \
    'data/mmMax.dat' w l title 'Maximum'
