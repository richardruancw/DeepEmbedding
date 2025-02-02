import random,sys,snap

def constructNet(filename, sep, weighted = False):
	weightMap = {}
	f = open(filename, "r")
	G = snap.TUndirNet.New()
	count = 0
	for line in f:
		count+=1
		if len(line) == 0:
			continue
		info = line.split(sep)
		node1 = int(info[0])
		node2 = int(info[1])
		if not G.IsNode(node1):
			G.AddNode(node1)
		if not G.IsNode(node2):
			G.AddNode(node2)
		if not G.IsEdge(node1, node2):
			G.AddEdge(node1, node2)
			if weighted:
				weight = int(info[2])
				weightMap[(node1, node2)] = weight
			else:
				weightMap[(node1, node2)] = 1
		if count%1000 == 0:
			print count
	print count
	return G, weightMap

def generateNeg(G,negLinks,seen, numNeg, numPos):
	while numNeg < numPos:
		node1 = G.GetRndNId()
		node2 = G.GetRndNId()

		if node1 > node2:
			temp = node1
			node1 = node2
			node2 = temp

		if (not G.IsEdge(node1,node2)) and ((node1,node2) not in seen) and (node1 != node2):
			negLinks.append([str(node1), str(node2), str(0)])
			numNeg+=1
			seen[(node1,node2)] = 0

	return negLinks

def generatePos(G, posRemovable, posNonRemovable, weightMap):
	for EI in G.Edges():
		
		source = EI.GetSrcNId()
		dest = EI.GetDstNId()
		if (source, dest) not in weightMap:
			weight = weightMap[(dest, source)]
		else:
			weight = weightMap[(source, dest)]

		if G.GetNI(source).GetDeg() > 1 and G.GetNI(dest).GetDeg() > 1:
			posRemovable.append([str(source), str(dest),str(weight),str(1)])
		else:
			posNonRemovable.append([str(source), str(dest),str(weight),str(1)])

	return posRemovable, posNonRemovable

def writeTrainTestFile(k, posRemovable, posNonRemovable, negLinks):
	random.shuffle(posNonRemovable)
	random.shuffle(negLinks)

	negIndices = (range(k)*(len(negLinks)/k + 1))[:len(negLinks)]

	for i in xrange(k):
		random.shuffle(posRemovable)
		train = open("eval/train"+str(i), "w")
		test = open("eval/test"+str(i), "w")

		node2vec = open("graph/node2vec"+str(i),"w")
		
		# first write out negative samples
		for index in xrange(len(negLinks)):
			if negIndices[index] != i:
				train.write(" ".join(negLinks[index])+"\n")
			else:
				test.write(" ".join(negLinks[index])+"\n")

		# write out removable links: first (len(posRemovable)+len(posNonRemovable))/k
		# of them should be in test set, the rest in train set
		for index in xrange(len(posRemovable)):
			if index < (len(posRemovable)+len(posNonRemovable))/k:
				test.write(" ".join([posRemovable[index][m] for m in xrange(4) if m != 2])+"\n")
			else:
				train.write(" ".join([posRemovable[index][m] for m in xrange(4) if m != 2])+"\n")
				node2vec.write(" ".join(posRemovable[index][:-1])+"\n")

		#write out non-removable links: they should all be in train set:
		for index in xrange(len(posNonRemovable)):
			train.write(" ".join([posNonRemovable[index][m] for m in xrange(4) if m != 2])+"\n")
			node2vec.write(" ".join(posNonRemovable[index][:-1])+"\n")
			# node2vec.write(" ".join(posNonRemovable[index])+"\n")
		
		train.close()
		test.close()
		node2vec.close()
	print "done!"

def split_graph(k, input_graph):
	G, weightMap = constructNet(input_graph, " ")
	numPos = G.GetEdges()
	numNeg = 0
	seen = {}
	negLinks = []
	posRemovable = []
	posNonRemovable = []
	negLinks = generateNeg(G,negLinks,seen,numNeg,numPos)
	del(seen)
	posRemovable, posNonRemovable = generatePos(G, posRemovable, posNonRemovable, weightMap)
	writeTrainTestFile(k, posRemovable, posNonRemovable, negLinks)

	
if __name__ == "__main__":
	args = sys.argv
	if len(args) != 3:
		print "Usage: python split.py number_of_partitions input_graph"
		sys.exit()
	k = int(sys.argv[1])
	input_graph = sys.argv[2]

	G, weightMap = constructNet(input_graph, " ")
	numPos = G.GetEdges()
	numNeg = 0
	seen = {}
	negLinks = []
	posRemovable = []
	posNonRemovable = []
	negLinks = generateNeg(G,negLinks,seen,numNeg,numPos)
	del(seen)
	posRemovable, posNonRemovable = generatePos(G, posRemovable, posNonRemovable, weightMap)
	writeTrainTestFile(k, posRemovable, posNonRemovable, negLinks)