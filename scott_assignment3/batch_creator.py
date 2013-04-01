''' ------------------------------------------
 - FILE: batch_creator.py
 - assignment3 - Parallel Matrix Multiply on the IBM Blue Gene/Q
 - Program written by Scott Todd, March 2013 for Parallel Programming and 
 -     Computing (CSCI 4320)
 - 
 - This script creates shell script batch files to be run via sbatch on the BG/Q
 -      batch-[64, 128, 256, 512, 1024]-[1, 2, 4]-mapping.sh
 -      batch-[64, 128, 256, 512, 1024]-[4].sh
 -
 - Submit these jobs on the BG/Q with "bash master_batch.sh"
 - 
 - Sample output file:
 -      #!/bin/bash
 -      #SBATCH --job-name=PCP3tddsMM64-2M
 -      #SBATCH -t 00:10:00
 -      #SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/
 -      #SBATCH -O
 -      #SBATCH -N 4
 -      #SBATCH -n 128
 -      #SBATCH -p small
 -
 -
 -      srun -o output-64-2-mapping.txt --partition=small --time=10 --nodes=4 --ntasks=128 --overcommit --runjob-opts="--mapping TEDCBA" ./mm
 - 
 - 
'''

CORES_PER_NODE = 16

for nodes in [4, 8, 16, 32, 64]:
    # mapping enabled
    for ranksPerCore in [1, 2, 4]:
        tasks = nodes*CORES_PER_NODE*ranksPerCore
        fname = "batch-"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+"-mapping.sh"
        file = open( fname, "wb" )
        
        file.write( "#!/bin/bash\n" )
        file.write( "#SBATCH --job-name=PCP3tddsMM"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+"M\n" )
        file.write( "#SBATCH -t 00:10:00\n" )
        file.write( "#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/\n" )
        file.write( "#SBATCH -O\n" )
        file.write( "#SBATCH -N " + str(nodes) + "\n" )
        file.write( "#SBATCH -n " + str(tasks)+"\n")
        file.write( "#SBATCH -p small\n" )
        
        file.write( "\n\n" )
        
        file.write( "srun -o output-" + str(nodes*CORES_PER_NODE) + "-" + str(ranksPerCore) + "-mapping.txt --partition=small --time=10 --nodes=" + str(nodes) + " --ntasks=" + str(tasks) + " --overcommit --runjob-opts=\"--mapping TEDCBA\" ./mm\n" )
        
        file.close()
    
    # mapping disabled
    ranksPerCore = 4
    tasks = nodes*CORES_PER_NODE*ranksPerCore
    fname = "batch-"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+".sh"
    file = open( fname, "wb" )
    
    file.write( "#!/bin/bash\n" )
    file.write( "#SBATCH --job-name=PCP3tddsMM"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+"\n" )
    file.write( "#SBATCH -t 00:10:00\n" )
    file.write( "#SBATCH -D /gpfs/sb/data/PCP3/PCP3tdds/\n" )
    file.write( "#SBATCH -O\n" )
    file.write( "#SBATCH -N " + str(nodes) + "\n" )
    file.write( "#SBATCH -n " + str(tasks)+"\n")
    file.write( "#SBATCH -p small\n" )
    
    file.write( "\n\n" )
    
    file.write( "srun -o output-" + str(nodes*CORES_PER_NODE) + "-" + str(ranksPerCore) + ".txt --partition=small --time=10 --nodes=" + str(nodes) + " --ntasks=" + str(tasks) + " --overcommit ./mm\n" )
    
    file.close()
    
