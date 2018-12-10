#!/bin/bash

#SBATCH -N3 -n1 -t00:05:00 --qos=test
#SBATCH -J hostname
#SBATCH --output=hostname.out

hostname

exit $?
