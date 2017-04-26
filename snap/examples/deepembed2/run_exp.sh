#!/bin/bash 

GRAPH_NAME="facebook_combined.edgelist"

NUM_PARTITION=2

GRAPH_PREFIX="node2vec"
TRAIN_PREFIX="train"
TEST_PREFIX="test"

python split.py $GRAPH_NAME $NUM_PARTITIO

let "COUNT=$NUM_PARTITION - 1"
for i in `seq 0 $COUNT`;
do
        GRAPH_TRAIN=$GRAPH_PREFIX
        GRAPH_TRAIN+=$i
        ./deepembed2 -i:eval/$GRAPH_TRAIN -out:graphs_folder -o:./embeddings/outemb0.txt -stats:./stats/stats_hope.txt -l:10 -d:24 -p:0.3  -q:0.5 -v -s:20 -nc:100 -ut:0.3

done