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
    
