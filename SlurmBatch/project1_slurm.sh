#!/bin/bash

#SBATCH -N4 -n5 --qos=test
#SBATCH -J project1
#SBATCH --output=project1.out
#999999000

mpirun -n $1 /home/betterthandialup/BTDU/BetterThanDIalUp/Project_1/project1_cmd_input $2 $3 

exit $?
