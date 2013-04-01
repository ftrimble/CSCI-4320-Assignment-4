all:
	# ---------------------------------------
	# KRATOS
	mpicc -o mm -Wall mm.c -lm
	# Run with 'mpirun -np #tasks ./mm'
	# ---------------------------------------
	# ---------------------------------------
	# Blue Gene/Q
	# mpixlc -o mm -O3 mm.c -lm
	#Run with 'srun -o output-4-512-testing.txt --partition small --time 10 --nodes=4 --ntasks=256 --overcommit --runjob-opts="--mapping TEDCBA" ./mm' or equivalent
	# ---------------------------------------

kratos:
	mpicc -o mm -Wall mm.c -lm
	# Run with 'mpirun -np #tasks ./mm'
	
bgq:
	mpixlc -o mm -O3 mm.c -lm
	#Run with 'srun -o output-4-512-testing.txt --partition small --time 10 --nodes=4 --ntasks=256 --overcommit --runjob-opts="--mapping TEDCBA" ./mm' or equivalent
	
quick:
	mpicc -o mm_quick -Wall mm_quick.c -lm
	# Run with 'mpirun -np #tasks ./mm_quick'
	
clean:
	rm -rf *o mm
	rm -rf *o mm_quick
