''' ------------------------------------------
 - FILE: output_parser.py
 - assignment3 - Parallel Matrix Multiply on the IBM Blue Gene/Q
 - Program written by Scott Todd, March 2013 for Parallel Programming and 
 -     Computing (CSCI 4320)
 - 
 - This script parses output files in its current directory into a single file.
 -      output-[64, 128, 256, 512, 1024]-[1, 2, 4]-mapping.txt
 -      output-[64, 128, 256, 512, 1024]-[4].txt
 - 
 - Assumes that all output files are present and calls them by name, to
 - simplify processing. It could, with minor modifications, work with arbitrary
 - file names and output formats.
'''

CORES_PER_NODE = 16

output_file = open( "output.txt", "wb" )

output_file.write( "--MAPPING ENABLED--\n" )

for nodes in [4, 8, 16, 32, 64]:
    # ----------------------------------------------------------
    # ----------------------------------------------------------
    # mapping enabled
    for ranksPerCore in [1, 2, 4]:
        # ----------------------------------------------------------
        tasks = nodes*CORES_PER_NODE*ranksPerCore
        fname = "output-"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+"-mapping.txt"
        file = open( fname, "rb" )
        # ----------------------------------------------------------
        
        # rank 0/numranks:
        line = file.next()
        
        # Total execution time (s), #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[1] + " " )
        
        #                 min,       max,       avg
        line = file.next()
        
        # Isend time (s), #, #, #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[3] + " " ) # avg
        
        # Irecv time (s), #, #, #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[3] + " " ) # avg
        
        # Isend bandwidth (bytes/s), #, #, #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[1] + " " )
        output_file.write( chunks[2] + " " )
        output_file.write( chunks[3] + " " )
        
        # Irecv bandwidth (bytes/s), #, #, #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[1] + " " )
        output_file.write( chunks[2] + " " )
        output_file.write( chunks[3] + " " )
        
        # Multi time (s), #, #, #
        line = file.next()
        chunks = [x.strip() for x in line.split(',')] 
        output_file.write( chunks[1] + " " )
        output_file.write( chunks[2] + " " )
        output_file.write( chunks[3] )
        
        output_file.write( "\n" )
        
        file.close()
        
# ----------------------------------------------------------
# ----------------------------------------------------------
output_file.write( "\n\n--MAPPING DISABLED--\n" )

for nodes in [4, 8, 16, 32, 64]:
    
    # mapping disabled
    # ----------------------------------------------------------
    ranksPerCore = 4
    tasks = nodes*CORES_PER_NODE*ranksPerCore
    fname = "output-"+str(nodes*CORES_PER_NODE)+"-"+str(ranksPerCore)+".txt"
    file = open( fname, "rb" )
    # ----------------------------------------------------------
    
    # rank 0/numranks:
    line = file.next()
    
    # Total execution time (s), #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[1] + " " )
    
    #                 min,       max,       avg
    line = file.next()
        
    # Isend time (s), #, #, #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[3] + " " ) # avg
    
    # Irecv time (s), #, #, #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[3] + " " ) # avg

    # Isend bandwidth (bytes/s), #, #, #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[1] + " " )
    output_file.write( chunks[2] + " " )
    output_file.write( chunks[3] + " " )
    
    # Irecv bandwidth (bytes/s), #, #, #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[1] + " " )
    output_file.write( chunks[2] + " " )
    output_file.write( chunks[3] + " " )
    
    # Multi time (s), #, #, #
    line = file.next()
    chunks = [x.strip() for x in line.split(',')] 
    output_file.write( chunks[1] + " " )
    output_file.write( chunks[2] + " " )
    output_file.write( chunks[3] )
    
    output_file.write( "\n" )
    
    file.close()
    

