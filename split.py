import random,sys

links = []
k = sys.argv[1]

with open("graph.txt", "r") as total:
	links = total.readlines()

random.shuffle(links)
indices = (range(k)*(len(links)/k + 1))[:len(links)]

for i in xrange(k):
	train = open("train"+str(i), "w")
	test = open("test"+str(i), "w")

	for index in xrange(len(links)):
		if indices[index] != i:
			train.write(links[index])
		else:
			test.write(links[index])
	
	train.close()
	test.close()



