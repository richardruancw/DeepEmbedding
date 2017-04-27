import numpy as np
import os, sys, logging

def ReadTimeCost(output_stats_path, origin = False):
	if origin:
		with open(os.path.join(output_stats_path, "OriginStats.txt"),"r") as f:
			for line in f:
				return float(line)

	with open(os.path.join(output_stats_path, "stats.txt"),"r") as f:
		count = 0
		for line in f:
			count += 1
			if count == 1:
				partition_time = float(line)
			elif count == 2:
				super_net_time = float(line)
			else:
				small_net_time = [float(x) for x in line.split()]
	return partition_time, super_net_time, np.max(small_net_time)

if __name__ == "__main__":
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"

	partition_time, super_net_time, small_net_max = ReadTimeCost(output_stats_path)
	origin_time = ReadTimeCost(output_stats_path, True)

	with open(os.path.join(output_stats_path, "cpp_time_cost"),"a") as f:
		f.write(" ".join([str(x) for x in [partition_time, super_net_time, small_net_max, origin_time]]))
		f.write("\n")