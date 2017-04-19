import numpy as np
import os
from sklearn.linear_model import SGDClassifier
from sklearn.metrics import confusion_matrix

"""
path and batch size need to be re-specified


"""
data_path = "data/"
batch_size_train = int(input("batch size?"))
# batch_size_test = 10000
d = 24
num_embeddings = 300

"""
input format: (id1, id2, label)
embedding format: (id em[1] em[2] ...)

"""

def load_train_test(data_path, our_embedding = True):
	train = np.loadtxt(os.path.join(data_path,"train0"))
	test = np.loadtxt(os.path.join(data_path,"test0"))
	batch_size_test = test.shape[0]
	if our_embedding:
		embeddings = np.loadtxt(os.path.join(data_path,"embedding0"), skiprows = 1)
	else:
		embeddings = np.loadtxt(os.path.join(data_path,"Origin"), skiprows = 1)

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

	print "accuracy: ", model.score(test_features, test_labels)


def train_and_test(data_path):
	train_x, train_y, test_x, test_y, embedding_map, batch_size_test = load_train_test(data_path)
	model = SGDClassifier()

	indices_bag_train = giveBatchIndices(batch_size_train, train_x.shape[0])
	indices_bag_test = giveBatchIndices(batch_size_test, test_x.shape[0])

	for i in xrange(len(indices_bag_train)):
		for j in xrange(len(indices_bag_test)):
			train_and_test_on_batch(model, train_x, train_y, test_x, test_y, embedding_map, indices_bag_train[i], indices_bag_test[j])

def generate_fake_data(data_path):
	id1 = np.random.randint(0,num_embeddings-1,batch_size_train)
	id2 = np.random.randint(0,num_embeddings-1,batch_size_train)
	y = np.random.randint(0,2,batch_size_train)
	data = np.column_stack((id1,id2,y))

	embeddings = np.column_stack((range(-1, num_embeddings),np.random.rand(num_embeddings+1,d)))
	
	np.savetxt(os.path.join(data_path, "train"), data)
	np.savetxt(os.path.join(data_path, "test"), data)
	np.savetxt(os.path.join(data_path, "embeddings"), embeddings)

if __name__ == "__main__":
	# generate_fake_data(data_path)
	train_and_test(data_path)
