gcloud dataproc jobs submit pyspark --cluster my_cluster collapse.py

gsutil cp gs://my-bucket/graph .
cd graph
cat part* > collapsed_graph.txt

python split.py 10