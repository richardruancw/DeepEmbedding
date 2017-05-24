import numpy as np
import os, sys, logging

def get_result_summary(graph_abbr, num_com):
	print("Gathering result summary for {}, with {} partitions".format(graph_abbr, num_com))