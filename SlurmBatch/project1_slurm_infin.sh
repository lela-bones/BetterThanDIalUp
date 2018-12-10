#!/bin/bash

#SBATCH -N4 -n5 --qos=test
#SBATCH -J project1
#SBATCH --output=project1.out

mpirun -n $1 /home/betterthandialup/BTDU/BetterThanDIalUp/Project_1/project1_infin_001 $2 $3 $4

exit $0
