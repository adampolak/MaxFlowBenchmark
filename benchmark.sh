#!/bin/sh

PROGRAM_FILE=./build/MaxFlowBenchmark
GRAPH_FILE=./data/synth.txt
NUM_TRAIN_SAMPLES=3
NUM_TEST_SAMPLES=5

ADD_TO_RES_FILENAME=228

declare -a arr=("0.0001" "0.01" "0.1" "1" "4" "16")

mkdir results

for i in "${arr[@]}"
do
  echo ${GRAPH_FILE} "$i"
  ${PROGRAM_FILE} "learn" ${GRAPH_FILE} ${NUM_TRAIN_SAMPLES} $i > "learnedsynth.txt"
  ${PROGRAM_FILE} "algo" ${GRAPH_FILE} "learnedsynth.txt" results/res_${ADD_TO_RES_FILENAME}_$i.csv ${NUM_TEST_SAMPLES} $i
done



#X = [0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 2, 4, 8, 16, 32]

