#!/bin/bash

#SBATCH -N4 -n10 --qos=test
#SBATCH -J project1
#SBATCH --output=project1.out
#999999000

mpirun -n $1 python /home/betterthandialup/BTDU/BetterThanDIalUp/Project3/proj.py 

exit $?
