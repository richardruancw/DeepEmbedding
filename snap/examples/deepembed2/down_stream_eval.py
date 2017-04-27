import numpy as np
import os, sys, logging
from sklearn.linear_model import SGDClassifier
from sklearn.metrics import confusion_matrix, f1_score

"""
input format: (id1, id2, label)
embedding format: (id em[1] em[2] ...)

"""
def parseArgs(args):
	
	batch_size = int(args[1].split(":")[1])
	d = int(args[2].split(":")[1])
	game_round = args[3].split(":")[1]
	ours =int(args[4].split(":")[1])

	return batch_size, d, game_round, ours

def load_train_test(eval_path, embedding_path, ours):
	train = np.loadtxt(os.path.join(eval_path,"train"+game_round))
	test = np.loadtxt(os.path.join(eval_path,"test"+game_round))
	batch_size_test = test.shape[0]
	if ours == 1:
		embeddings = np.loadtxt(os.path.join(embedding_path,"outemb"+game_round+".txt"), skiprows = 1)
		 # embeddings = np.loadtxt(os.path.join(embedding_path,"outemb_lonely0.txt"), skiprows = 1)
	else:
		embeddings = np.loadtxt(os.path.join(embedding_path,"Origin"), skiprows = 1)

	embedding_map = {}

	for i in xrange(embeddings.shape[0]):
		if embeddings[i,0] not in embedding_map:
			embedding_map[embeddings[i,0]] = embeddings[i,1:]
	
	del embeddings		
	
	train_x = train[:,:train.shape[1]-1]
	train_y = train[:,train.shape[1]-1]

	test_x = test[:,:test.shape[1]-1]
	test_y = test[:,test.shape[1]-1]

	return train_x, train_y, test_x, test_y, embedding_map, batch_size_test

def giveBatchIndices(batchSize, nRows):
    indices = range(nRows)
    np.random.shuffle(indices)
    ret = []
    start = 0
    while start < nRows:
        ret.append(indices[start:(start+batchSize)])
        start += batchSize
    return ret

def get_batch_features(train_x, train_y, test_x, test_y, embedding_map, batch_indices_train, batch_indices_test):

	train_ids = train_x[batch_indices_train,:].astype(int)
	train_label_np = train_y[batch_indices_train]

	test_ids = test_x[batch_indices_test,:].astype(int)
	test_label_np = test_y[batch_indices_test]

	vecs1_train = []
	vecs2_train = []
	train_labels = []
	for i in xrange(train_ids.shape[0]):
		if train_ids[i,0] in embedding_map and train_ids[i,1] in embedding_map:
			vecs1_train.append(embedding_map[train_ids[i,0]])
			vecs2_train.append(embedding_map[train_ids[i,1]])
			train_labels.append(train_label_np[i])

	vecs1_train = np.array(vecs1_train)
	vecs2_train = np.array(vecs2_train)
	train_labels = np.array(train_labels)

	vecs1_test = []
	vecs2_test = []
	test_labels = []
	for i in xrange(test_ids.shape[0]):
		if test_ids[i,0] in embedding_map and test_ids[i,1] in embedding_map:
			vecs1_test.append(embedding_map[test_ids[i,0]])
			vecs2_test.append(embedding_map[test_ids[i,1]])
			test_labels.append(test_label_np[i])

	vecs1_test = np.array(vecs1_test)
	vecs2_test = np.array(vecs2_test)
	test_labels = np.array(test_labels)
	
	train_features = vecs1_train*vecs2_train
	test_features = vecs1_test*vecs2_test

	return train_features, train_labels, test_features, test_labels

def train_and_test_on_batch(model, train_x, train_y, test_x, test_y, embedding_map, batch_indices_train, batch_indices_test, confusion_mat = False):
	train_features, train_labels,test_features, test_labels = get_batch_features(train_x, train_y, test_x, test_y, embedding_map, batch_indices_train, batch_indices_test)
	model.partial_fit(train_features, train_labels, [0,1])

	predictions = model.predict(test_features)

	if confusion_mat:
		print confusion_matrix(test_labels, predictions, labels = [0,1])

	accuracy = model.score(test_features, test_labels)
	f1 = f1_score(test_labels,predictions)

	return accuracy, f1

def train_and_test(eval_path, embedding_path, ours):
	logger.info("Loading train test data and embeddings")
	train_x, train_y, test_x, test_y, embedding_map, batch_size_test = load_train_test(eval_path, embedding_path,ours)
	model = SGDClassifier()

	indices_bag_train = giveBatchIndices(batch_size, train_x.shape[0])
	indices_bag_test = giveBatchIndices(test_x.shape[0], test_x.shape[0])
	logger.info("Start training and testing")
	
	for i in xrange(len(indices_bag_train)):
		for j in xrange(len(indices_bag_test)):
			accuracy, f1 = train_and_test_on_batch(model, train_x, train_y, test_x, test_y, embedding_map, indices_bag_train[i], indices_bag_test[j])
			logger.info("accuracy: %f", accuracy)
			logger.info("f1: %f", f1)
			print "accuracy: ", accuracy
			print "f1: ", f1
			print "-------------------------------------"
	return accuracy, f1

# def generate_fake_data(data_path):
# 	num_embeddings = 300
# 	id1 = np.random.randint(0,num_embeddings-1,batch_size_train)
# 	id2 = np.random.randint(0,num_embeddings-1,batch_size_train)
# 	y = np.random.randint(0,2,batch_size_train)
# 	data = np.column_stack((id1,id2,y))

# 	embeddings = np.column_stack((range(-1, num_embeddings),np.random.rand(num_embeddings+1,d)))
	
# 	np.savetxt(os.path.join(data_path, "train"), data)
# 	np.savetxt(os.path.join(data_path, "test"), data)
# 	np.savetxt(os.path.join(data_path, "embeddings"), embeddings)

if __name__ == "__main__":
	# generate_fake_data(data_path)
	eval_path = "eval/"
	embedding_path = "embeddings/"
	output_stats_path = "stats/"
	
	logger = logging.getLogger(__name__)
	logger.setLevel(logging.INFO)
	handler = logging.FileHandler(os.path.join(output_stats_path,'eval.log'))
	formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
	handler.setFormatter(formatter)
	logger.addHandler(handler)

	args = sys.argv
	if len(args) != 5:
		print "Usage: python down_stream_eval.py -b:batch_size -d:feature_dim -r:which_round -ours:0or1"
		sys.exit()

	batch_size, d, game_round, ours = parseArgs(args)
	accuracy, f1 = train_and_test(eval_path, embedding_path, ours)
	if ours == 1:
		with open(os.path.join(output_stats_path, "down_stream_results"),"a") as f:
			f.write(" ".join([game_round, str(accuracy), str(f1)]))
			f.write("\n")
	else:
		with open(os.path.join(output_stats_path, "down_stream_results_origin"),"a") as f:
			f.write(" ".join([game_round, str(accuracy), str(f1)]))
			f.write("\n")

