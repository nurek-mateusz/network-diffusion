#!/usr/bin/env bash
set -ex

# This is the master script for the capsule. When you click "Reproducible Run", the code in this file will execute.

# How much data should be in the test set. Range [0,1]
DIVIDE_RATIO=0.2

DATASET_NAME="hypertext"

# inform about starting the scripts
echo "### BEGIN OF EXECUTION ###"

mkdir -p results/cogsnet/$DATASET_NAME

# compile the code
make

# prepare a script for computations (parameters described in cogsnet-prepare.c)
cogsnet/cogsnet-prepare cogsnet/cogsnet-compute-$DATASET_NAME.sh data/params/params-$DATASET_NAME.csv data/train_test/$DATASET_NAME-train-$DIVIDE_RATIO.csv results/cogsnet/$DATASET_NAME /bin/bash 0

# run the experiments - this can be pararellised as each run is independent from others
cogsnet/cogsnet-compute-$DATASET_NAME.sh

# inform about the end of analyses
echo "### END OF EXECUTION ###"
