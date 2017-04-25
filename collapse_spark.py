from pyspark import SparkContext

sc = SparkContext()

table1 = sc.textFile("gs://pinterest-data/pins.tsv").map(lambda line: line.split("\t")).map(lambda list: (list[2], list[1]))
table2 = sc.textFile("gs://pinterest-data/pins.tsv").map(lambda line: line.split("\t")).map(lambda list: (list[2], list[1]))

graph = table1.join(table2).map(lambda (board, (pin1, pin2)): tuple(sorted((pin1, pin2)))).filter(lambda pins : pins[0] != pins[1])\
.map(lambda pins: (pins, 1)).distinct().reduceByKey(lambda v1,v2: v1+v2).map(lambda (pair, count): " ".join([pair[0], pair[1], str(count)]))

graph.saveAsTextFile("gs://pinterest-data/graph")

sc.stop()
