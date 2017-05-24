import numpy as np
import os, sys, logging

def ReadTempTimeCost(output_stats_path, origin = False):
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
	return partition_time, super_net_time, np.sum(small_net_time), np.mean(small_net_time)

def collect_temp_cpp_time():
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"

	partition_time, super_net_time, small_net_sum, small_net_mean = ReadTempTimeCost(output_stats_path)
	origin_time = ReadTempTimeCost(output_stats_path, True)

	with open(os.path.join(output_stats_path, "cpp_time_cost"),"a") as f:
		f.write(" ".join([str(x) for x in [partition_time, super_net_time, small_net_sum, small_net_mean, origin_time]]))
		f.write("\n")
	
def ReadEvaluation(output_stats_path, post_fix):
	acc, f1 = {}, {}
	with open(os.path.join(output_stats_path, "down_stream_results" + post_fix),"r") as f:
		for line in f:
			content = line.split()
			res = [float(x) for x in content[0:3]]
			signature = content[3]
			if signature not in acc:
				acc[signature] = [res[1]]
			else:
				acc[signature].append(res[1])
			if signature not in f1:
				f1[signature] = [res[2]]
			else:
				f1[signature].append(res[2])
	acc_mean_out, acc_std_out = {}, {}

	for k, v in acc.items():
		acc_mean_out[k] = np.mean(v)
		acc_std_out[k] = np.std(v)
	f1_mean_out, f1_std_out = {}, {}
	for k, v in f1.items():
		f1_mean_out[k] = np.mean(v)
		f1_std_out[k] = np.std(v)	

	return acc_mean_out, acc_std_out, f1_mean_out, f1_std_out

def ReadTimeCost(output_stats_path):
	parition, super_net, small_net_sum, small_net_mean, origin = [], [], [], [], []
	with open(os.path.join(output_stats_path, "cpp_time_cost"),"r") as f:
		for line in f:
			res = [float(x) for x in line.split()]
			parition.append(res[0])
			super_net.append(res[1])
			small_net_sum.append(res[2])
			small_net_mean.append(res[3])
			origin.append(res[4])
	return np.mean(parition), np.std(parition), np.mean(super_net), np.std(super_net), np.mean(small_net_sum), np.std(small_net_sum), np.mean(small_net_mean), np.std(small_net_mean), np.mean(origin), np.std(origin)

def get_result_summary(file_name):
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"

	partition_time_mean, partition_time_std, super_net_time_mean, super_net_time_std, small_net_sum_mean, small_net_sum_std, small_net_mean_mean, small_net_mean_std, origin_mean, origin_std = ReadTimeCost(output_stats_path)

	with open(os.path.join(output_stats_path, file_name), "w") as f:
		acc_mean_out, acc_std_out, f1_mean_out, f1_std_out = ReadEvaluation(output_stats_path, "")
		f.write("Our methods: \n")
		for k in acc_mean_out.keys():
			f.write(" ".join([k, "\n"]))
			acc_mean, acc_std = acc_mean_out[k], acc_std_out[k]
			f1_mean, f1_std = f1_mean_out[k], f1_std_out[k]
			f.write(" ".join(["acc:", str(acc_mean), "acc std:", str(acc_std), "f1 mean:", str(f1_mean), "f1 std:", str(f1_std)]))
			f.write("\n")
		
		acc_mean_out, acc_std_out, f1_mean_out, f1_std_out = ReadEvaluation(output_stats_path, "_origin")
		f.write("Origin node2vec\n")
		for k in acc_mean_out.keys():
			acc_mean, acc_std = acc_mean_out[k], acc_std_out[k]
			f1_mean, f1_std = f1_mean_out[k], f1_std_out[k]
			f.write(" ".join(["acc:", str(acc_mean), "acc std:", str(acc_std), "f1 mean:", str(f1_mean), "f1 std:", str(f1_std)]))
			f.write("\n")

		f.write("Our methods, time cost:\n")
		f.write(" ".join(["parition mean time:", str(partition_time_mean), "partition time std:", str(partition_time_std), "super net mean time:", str(super_net_time_mean),
			"super net time std:", str(super_net_time_std), "small net sum mean:", str(small_net_sum_mean), "small net sum std:", str(small_net_sum_std), "small net mean mean:", str(small_net_mean_mean),
			"small net mean std:", str(small_net_mean_std)]))
		f.write("\n")
		f.write("The origin method, time cost\n")
		f.write(" ".join(["origin mean time:", str(origin_mean), "origin time std:", str(origin_std)]))
		f.write("\n")








