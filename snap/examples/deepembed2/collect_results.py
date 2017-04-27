import numpy as np
import os, sys, logging

"""
input format: (id1, id2, label)
embedding format: (id em[1] em[2] ...)

"""

def ParseArgs(args):
	num_fold = int(args[1].split(":")[1])
	num_com = int(args[2].split(":")[1])
	return num_fold, num_com
	
def ReadEvaluation(output_stats_path):
	acc, f1 = [], []
	with open(os.path.join(output_stats_path, "down_stream_results"),"r") as f:
		for line in f:
			res = [double(x) for x in line.split()]
			acc.apend(res[1])
			f1.append(res[2])
	return np.mean(acc), np.std(acc), np.mean(f1), np.std(f1)

def ReadTimeCost(output_stats_path):

	with open(os.path.join(output_stats_path, "stats.txt"),"r") as f:
		count = 0
		for line in f:
			count += 1
			if count == 1:
				partition_time = double(line)
			elif count == 2:
				super_net_time = double(line)
			else:
				small_net_time = [double(x) for x in line.split()]
	return partition_time, super_net_time, np.mean(small_net_time), np.std(small_net_time), np.max(small_net_time)




if __name__ == "__main__":
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"

	acc_mean, acc_std, f1_mean, f1_std = ReadEvaluation(output_stats_path)
	partition_time, super_net_time, small_net_mean, small_net_std, small_net_max = ReadTimeCost(output_stats_path)

	

	
	
