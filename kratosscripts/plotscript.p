set terminal post eps
set logscale x 2

################################
##### Execution time plots #####
################################

set output 'plots/exec.eps'
set xlabel 'Number of Threads per Node'
set ylabel 'Execution Time (seconds)'
set title 'Execution Time vs. Number of Threads Per Node'
plot 'data/bluegene/execAvg.dat' w l title 'Average', \
    'data/bluegene/execMin.dat' w l title 'Minimum',  \
    'data/bluegene/execMax.dat' w l title 'Maximum'

#############################
##### Matrix Size Plots #####
#############################

# set output 'plots/sizes.eps'
# set ylabel 'Execution Time (seconds)'
# set xlabel 'Matrix Size'
# set title 'Execution Time vs. Matrix Size'
# plot 'data/sizes.dat' w l title 'Average times per node'

##########################
##########################
###### Kratos Plots ######
##########################
##########################

################################
##### Execution Time plots #####
################################

set output 'plots/execK.eps'
set xlabel 'Number of Threads per Node'
set ylabel 'Execution Time (seconds)'
set title 'Execution Time vs. Number of Threads Per Node'
plot 'data/kratos/execAvg.dat' w l title 'Average', \
    'data/kratos/execMin.dat' w l title 'Minimum',  \
    'data/kratos/execMax.dat' w l title 'Maximum'
