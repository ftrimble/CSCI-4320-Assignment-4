 # setting output to eps files for use with latex 
set terminal eps
set logscale y 2
set logscale x 2
set xlabel 'Number of Tasks'

####################################################
####################################################
##### outputting data from kratos to plots     #####
####################################################
####################################################

################################
##### Execution time plots #####
################################

set output 'plots/execAvg.eps'
set ylabel 'Execution Time ( seconds )'
set title 'Execution Time vs. Number of Tasks'
    plot 'data/execAvg.dat' w l title 'Average', \
    'data/execMin.dat' w l title 'Minimum', \
    'data/execMax.dat' w l title 'Maximum'

###################################################
##### Effective communication bandwidth plots #####
###################################################

set output 'plots/commAvg.eps'
set ylabel 'Communication Bandwidth ( bytes/second )'
set title 'Communication Bandwidth vs. Number of Tasks'
plot 'data/commAvg.dat' w l title 'Average', \
    'data/commMin.dat' w l title 'Minimum', \
    'data/commMax.dat' w l title 'Maximum'

######################################
##### Matrix Multiply time plots #####
######################################

set output 'plots/mmAvg.eps'
set ylabel 'Matrix Multiply Time ( seconds )'
set title 'Matrix Multiply Time vs. Number of Tasks'
plot 'data/mmAvg.dat' w l title 'Average', \
    'data/mmMin.dat' w l title 'Minimum',  \
    'data/mmMax.dat' w l title 'Maximum'

################################################################
################################################################
##### outputting data from experiment 1 and 2 to plots     #####
################################################################
################################################################

################################
##### Execution time plots #####
################################

set output 'plots/bluegene/experiment1/execAvg.eps'
set ylabel 'Avg Execution Time ( seconds )'
set title 'Execution Time vs. Number of Tasks'
plot 'data/bluegene/experiment1/execAvg.dat' w l title 'Experiment 1 Average', \
    'data/bluegene/experiment1/execMin.dat' w l title 'Experiment 1 Minimum',  \
    'data/bluegene/experiment1/execMax.dat' w l title 'Experiment 1 Maximum',  \
    'data/bluegene/experiment2/execAvg.dat' w l title 'Experiment 2 Average',  \
    'data/bluegene/experiment2/execMin.dat' w l title 'Experiment 2 Minimum',  \
    'data/bluegene/experiment2/execMax.dat' w l title 'Experiment 2 Maximum'

###################################################
##### Effective communication bandwidth plots #####
###################################################

set output 'plots/bluegene/experiment1/commAvg.eps'
set ylabel 'Avg Communication Bandwidth ( bytes/second )'
set title 'Communication Bandwidth vs. Number of Tasks'
plot 'data/bluegene/experiment1/commAvg.dat' w l title 'Experiment 1 Average', \
    'data/bluegene/experiment1/commMin.dat' w l title 'Experiment 1 Minimum',  \
    'data/bluegene/experiment1/commMax.dat' w l title 'Experiment 1 Maximum',  \
    'data/bluegene/experiment2/commAvg.dat' w l title 'Experiment 2 Average',  \
    'data/bluegene/experiment2/commMin.dat' w l title 'Experiment 2 Minimum',  \
    'data/bluegene/experiment2/commMax.dat' w l title 'Experiment 2 Maximum'

######################################
##### Matrix Multiply time plots #####
######################################

set output 'plots/bluegene/experiment1/mmAvg.eps'
set ylabel 'Avg Matrix Multiply Time ( seconds )'
set title 'Matrix Multiply Time vs. Number of Tasks'
plot 'data/bluegene/experiment1/mmAvg.dat' w l title 'Experiment 1 Average', \
    'data/bluegene/experiment1/mmMin.dat' w l title 'Experiment 1 Minimum',  \
    'data/bluegene/experiment1/mmMax.dat' w l title 'Experiment 1 Maximum',  \
    'data/bluegene/experiment2/mmAvg.dat' w l title 'Experiment 2 Average',  \
    'data/bluegene/experiment2/mmMin.dat' w l title 'Experiment 2 Minimum',  \
    'data/bluegene/experiment2/mmMax.dat' w l title 'Experiment 2 Maximum'
