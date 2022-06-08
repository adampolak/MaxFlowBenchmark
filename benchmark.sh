#!/bin/bash

PROGRAM_FILE=./cmake-build-debug/MaxFlowBenchmark

declare -a graph_files=("data/LB07-bunny-sml.max")
#declare -a graph_files=("data/synth2.txt")
#declare -a train_samples=("1" "3")
#declare -a test_samples=("1" "3" "5")
#declare -a stds=("0" "0.01" "1" "4")
declare -a train_samples=("1")
declare -a test_samples=("3")
declare -a stds=("0" "0.01")

rm -r results

mkdir results
mkdir results/data

for GRAPH_FILE in "${graph_files[@]}"
do
  for NUM_TRAIN_SAMPLES in "${train_samples[@]}"
  do
    for NUM_TEST_SAMPLES in "${test_samples[@]}"
    do
      for i in "${stds[@]}"
      do
        echo ${GRAPH_FILE} "$i" "$NUM_TRAIN_SAMPLES" "$NUM_TEST_SAMPLES"
        ${PROGRAM_FILE} "learn" ${GRAPH_FILE} ${NUM_TRAIN_SAMPLES} $i > "learnedsynth.txt"
        ${PROGRAM_FILE} "algo" ${GRAPH_FILE} "learnedsynth.txt" results/${GRAPH_FILE}_${NUM_TRAIN_SAMPLES}_${NUM_TEST_SAMPLES}_$i.csv ${NUM_TEST_SAMPLES} $i
      done
    done
  done
done

#X = [0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 2, 4, 8, 16, 32]

