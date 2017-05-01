#!/bin/bash 

DO_PARTITION=1

# Learning n2v
GRAPH_NAME=facebook_combined.edgelist
RESULT_NAME=test_result

NUM_PARTITION=2
NUM_DIM=24
NUM_SUPER_DIM=24

NUM_Q_n2v=0.5
NUM_P_n2v=0.3
NUM_WALKLEN=30
NUM_COM=100
NUM_UPDATE_RATE=0.3
MERGE_THRESHOLD=0.4
COMMUNITY_DETECTION_OPTION=2
# Evaluation
NUM_BATCH_SIZE=10000
# 0: regular embedding; 1: decompose the embedding as S, N, S + H for comparisons
CHOICE_EMBED_DECOMPOSE=1

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

if [ $DO_PARTITION -eq 1 ]
then
	python split.py $NUM_PARTITION $GRAPH_FULL_PATH
fi

cd stats
rm *
cd ..

let "COUNT=$NUM_PARTITION - 1"
for i in `seq 0 $COUNT`;
do
        GRAPH_TRAIN="$GRAPH_PREFIX$i"
        # run our node2vec on this training graph
        EMBEDDING_TRAIN="$EMBEDDING_PREFIX$i$TXT"
        ./deepembed2 -i:graph/$GRAPH_TRAIN -out:graphs_folder -o:./embeddings/$EMBEDDING_TRAIN -stats:./stats/stats.txt -l:$NUM_WALKLEN -d:$NUM_DIM -p:$NUM_P_n2v  -q:$NUM_Q_n2v $CHOICE_VERBOSE -nc:$NUM_COM -ut:$NUM_UPDATE_RATE $CHOICE_DIRECTED $CHOICE_WEIGHTED -ours:1 -mt:$MERGE_THRESHOLD -cdo:$COMMUNITY_DETECTION_OPTION -sd:$NUM_SUPER_DIM
        ./deepembed2 -i:graph/$GRAPH_TRAIN -out:graphs_folder -o:./embeddings/$EMBEDDING_TRAIN -stats:./stats/stats.txt -l:$NUM_WALKLEN -d:$NUM_DIM -p:$NUM_P_n2v  -q:$NUM_Q_n2v $CHOICE_VERBOSE -nc:$NUM_COM -ut:$NUM_UPDATE_RATE $CHOICE_DIRECTED $CHOICE_WEIGHTED -ours:0
        python get_time_cpp.py
        echo "This is our method"
        python down_stream_eval.py -b:$NUM_BATCH_SIZE -d:$NUM_DIM -r:$i -ours:1 -decomp:$CHOICE_EMBED_DECOMPOSE -sd:$NUM_SUPER_DIM
        echo "The following is original node2vec" 
        python down_stream_eval.py -b:$NUM_BATCH_SIZE -d:$NUM_DIM -r:$i -ours:0 -decomp:0 -sd:$NUM_SUPER_DIM
done

python collect_results.py $RESULT_NAME

