import random,sys,snap

k = int(sys.argv[1])

G = snap.LoadEdgeList(snap.PNGraph, "../graph.txt", 0, 1, " ")
numPos = G.GetEdges()
numNeg = 0
seen = {}
negLinks = []
posRemovable = []
posNonRemovable = []

# for EI in G.Edges():
# 	links.append([str(EI.GetSrcNId()), str(EI.GetDstNId()),str(1)])
# 	print G.GetNI(EI.GetSrcNId()).GetDeg()

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

def generatePos(G, posRemovable, posNonRemovable):
	for EI in G.Edges():
		
		source = EI.GetSrcNId()
		dest = EI.GetDstNId()

		if G.GetNI(source).GetDeg() > 1 and G.GetNI(dest).GetDeg() > 1:
			posRemovable.append([str(source), str(dest),str(1)])
		else:
			posNonRemovable.append([str(source), str(dest),str(1)])

	return posRemovable, posNonRemovable

def writeTrainTestFile(k, posRemovable, posNonRemovable, negLinks):
	random.shuffle(posNonRemovable)
	random.shuffle(negLinks)

	negIndices = (range(k)*(len(negLinks)/k + 1))[:len(negLinks)]

	for i in xrange(k):
		random.shuffle(posRemovable)
		train = open("train"+str(i), "w")
		test = open("test"+str(i), "w")

		node2vec = open("node2vec"+str(i),"w")
		
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
				test.write(" ".join(posRemovable[index])+"\n")
			else:
				train.write(" ".join(posRemovable[index])+"\n")
				node2vec.write(" ".join(posRemovable[index][:2])+"\n")

		#write out non-removable links: they should all be in train set:
		for index in xrange(len(posNonRemovable)):
			train.write(" ".join(posNonRemovable[index])+"\n")
			node2vec.write(" ".join(posNonRemovable[index][:2])+"\n")
		
		train.close()
		test.close()
		node2vec.close()
	print "done!"
	
if __name__ == "__main__":	
	negLinks = generateNeg(G,negLinks,seen,numNeg,numPos)
	del(seen)
	posRemovable, posNonRemovable = generatePos(G, posRemovable, posNonRemovable)
	writeTrainTestFile(k, posRemovable, posNonRemovable, negLinks)