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
	
def ReadEvaluation(output_stats_path, post_fix):
	acc, f1 = [], []
	with open(os.path.join(output_stats_path, "down_stream_results" + post_fix),"r") as f:
		for line in f:
			res = [float(x) for x in line.split()]
			acc.append(res[1])
			f1.append(res[2])
	return np.mean(acc), np.std(acc), np.mean(f1), np.std(f1)

def ReadTimeCost(output_stats_path):
	parition, super_net, small_net_max, origin = [], [], [], []
	with open(os.path.join(output_stats_path, "cpp_time_cost"),"r") as f:
		for line in f:
			res = [float(x) for x in line.split()]
			parition.append(res[0])
			super_net.append(res[1])
			small_net_max.append(res[2])
			origin.append(res[3])
	return np.mean(parition), np.std(parition), np.mean(super_net), np.std(super_net), np.mean(small_net_max), np.std(small_net_max), np.mean(origin), np.std(origin)


if __name__ == "__main__":
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"

	acc_mean, acc_std, f1_mean, f1_std = ReadEvaluation(output_stats_path, "")
	acc_mean_origin, acc_std_origin, f1_mean_origin, f1_std_origin = ReadEvaluation(output_stats_path, "_origin")
	partition_time_mean, partition_time_std, super_net_time_mean, super_net_time_std, small_net_max_mean, small_net_max_std, origin_mean, origin_std = ReadTimeCost(output_stats_path)

	with open(os.path.join(output_stats_path, "combined_results"), "w") as f:
		f.write("Our methods\n")
		f.write(" ".join("acc:", acc_mean, "acc std:", acc_std, "f1 mean:", f1_mean, "f1 std:", f1_std))
		f.write("\n")
		f.write("Origin node2vec\n")
		f.write(" ".join("acc:", acc_mean_origin, "acc std:", acc_std_origin, "f1 mean:", f1_mean_origin, "f1 std:", f1_std_origin))
		f.write("\n")
		f.write("Our methods, time cost:\n")
		f.write(" ".join("parition mean time", partition_time_mean, "partition time std", partition_time_std, "super net mean time", super_net_time_mean,
			"super net time std", super_net_time_std, "small net max mean", small_net_max_mean, "small net max std", small_net_max_std))
		f.write("\n")
		f.write("The origin method, time cost\n")
		f.write(" ".join("origin mean time", origin_mean, "origin time std", origin_std))







	

	
	
