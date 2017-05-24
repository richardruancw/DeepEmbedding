import os
import gc
from subprocess import call
from split import split_graph

from exputils.time import collect_time_stats, get_time_summary
from exputils.result import get_result_summary
from exputils.partition import collect_partition_stats,  get_partition_summary

from down_stream_eval_extend import down_stream_eval

DO_PARTITION=True

# Learning n2v
GRAPH_NAME='com-amazon-ws.txt'
GRAPN_ABBR=GRAPH_NAME.split('-')[0]

RESULT_NAME='test_result'

NUM_PARTITION=2
NUM_DIM=48
NUM_SMALL_DIM=24
NUM_SUPER_DIM=24

NUM_Q_n2v=0.5
NUM_P_n2v=0.3
NUM_WALKLEN=30
NUM_COM=500
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
# If get raw communities from lowest degree node instead of random
CHOICE_SMART="-ss"

GRAPH_FULL_PATH='graph/' + GRAPH_NAME

GRAPH_PREFIX="node2vec"
TRAIN_PREFIX="train"
TEST_PREFIX="test"
EMBEDDING_PREFIX="outemb"

def delete_all_files(folder_path, keep_file = ['README.txt']):
	file_paths = os.listdir(folder_path)
	for path in file_paths:
		if path not in keep_file:
			os.remove(folder_path + '/' + path)

	
def main():
	if DO_PARTITION:
		delete_all_files('./embeddings')
		delete_all_files('./eval')
		split_graph(NUM_PARTITION, GRAPH_FULL_PATH)
	delete_all_files('./stats')

	gc.collect()

	for i in range(NUM_PARTITION):
		GRAPH_TRAIN = GRAPH_PREFIX + str(i)
		EMBEDDING_TRAIN = EMBEDDING_PREFIX + str(i) + '.txt'

		call_statement_our = './deepembed2 -i:graph/' + GRAPH_TRAIN + ' -out:graphs_folder' + ' -o:./embeddings/' + EMBEDDING_TRAIN + \
		' -stats:./stats/stats.txt' + ' -l:' + str(NUM_WALKLEN) + ' -d:' + str(NUM_SMALL_DIM) +  ' -p:' + str(NUM_P_n2v) + \
		' -q:' + str(NUM_Q_n2v) + ' ' + CHOICE_VERBOSE + ' -nc:' + str(NUM_COM) + ' -ut:' + str(NUM_UPDATE_RATE) + \
		' ' + CHOICE_DIRECTED + ' ' + CHOICE_WEIGHTED + ' ours:' + str(1) + ' -mt:' + str(MERGE_THRESHOLD) + \
		' -cdo:' +  str(COMMUNITY_DETECTION_OPTION) + ' -sd:' + str(NUM_SUPER_DIM) + ' ' + str(CHOICE_SMART)

		call_statement_original = './deepembed2 -i:graph/' + GRAPH_TRAIN + ' -out:graphs_folder' + ' -o:./embeddings/' + \
		EMBEDDING_TRAIN + ' -stats:./stats/stats.txt' + ' -l:' + str(NUM_WALKLEN) + ' -d:' + str(NUM_DIM) +  ' -p:' + \
		str(NUM_P_n2v) + ' -q:' + str(NUM_Q_n2v) + ' ' + CHOICE_VERBOSE + ' ' + CHOICE_DIRECTED + \
		' ' + CHOICE_WEIGHTED + ' -ours:' + str(0)

		call(call_statement_our, shell=True)

		# Collect temporal statistics for this fold for later analysis
		collect_partition_stats()
		collect_time_stats()

		call(call_statement_original, shell=True)

		print("This is our method")
		down_stream_eval(NUM_BATCH_SIZE, NUM_SMALL_DIM, i, 1, CHOICE_EMBED_DECOMPOSE, NUM_SUPER_DIM)

		print("This is the original node2vec")
		down_stream_eval(NUM_BATCH_SIZE, NUM_DIM, i, 0, CHOICE_EMBED_DECOMPOSE)

	# Gather informations from this cross-validation test
	get_time_summary(GRAPN_ABBR, NUM_COM)
	get_result_summary(GRAPN_ABBR, NUM_COM)
	get_partition_summary(GRAPN_ABBR, NUM_COM)

if __name__ == "__main__":
	main()