#!/bin/bash 

# Learning n2v
GRAPH_NAME=facebook_combined.edgelist
NUM_PARTITION=2
NUM_DIM=10
NUM_Q_n2v=0.5
NUM_P_n2v=0.3
NUM_WALKLEN=30
NUM_COM=100
NUM_UPDATE_RATE=0.3

# Evaluation
NUM_BATCH_SIZE=10000


# If weighted ="-w" else =""
CHOICE_WEIGHTED=""
# If weighted ="-dr" else =""
CHOICE_DIRECTED=""
# If verbose ="-v"
CHOICE_VERBOSE="-v"


GRAPH_FULL_PATH=graph/
GRAPH_FULL_PATH+=$GRAPH_NAME

GRAPH_PREFIX="node2vec"
TRAIN_PREFIX="train"
TEST_PREFIX="test"
EMBEDDING_PREFIX="outemb"

TXT=".txt"

python split.py $NUM_PARTITION $GRAPH_FULL_PATH

let "COUNT=$NUM_PARTITION - 1"
for i in `seq 0 $COUNT`;
do
        GRAPH_TRAIN="$GRAPH_PREFIX$i"
        # run our node2vec on this training graph
        EMBEDDING_TRAIN="$EMBEDDING_PREFIX$i$TXT"
        ./deepembed2 -i:graph/$GRAPH_TRAIN -out:graphs_folder -o:./embeddings/$EMBEDDING_TRAIN -stats:./stats/stats.txt -l:$NUM_WALKLEN -d:$NUM_DIM -p:$NUM_P_n2v  -q:$NUM_Q_n2v $CHOICE_VERBOSE -nc:$NUM_COM -ut:$NUM_UPDATE_RATE $CHOICE_DIRECTED $CHOICE_WEIGHTED -ours:1
        ./deepembed2 -i:graph/$GRAPH_TRAIN -out:graphs_folder -o:./embeddings/$EMBEDDING_TRAIN -stats:./stats/stats.txt -l:$NUM_WALKLEN -d:$NUM_DIM -p:$NUM_P_n2v  -q:$NUM_Q_n2v $CHOICE_VERBOSE -nc:$NUM_COM -ut:$NUM_UPDATE_RATE $CHOICE_DIRECTED $CHOICE_WEIGHTED -ours:0
        python get_time_cpp.py
        python down_stream_eval.py -b:$NUM_BATCH_SIZE -d:$NUM_DIM -r:$i -ours:1
        python down_stream_eval.py -b:$NUM_BATCH_SIZE -d:$NUM_DIM -r:$i -ours:0
done

python collect_results.py 
