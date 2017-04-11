from pyspark import SparkContext

sc = SparkContext()

table1 = sc.textFile("little_pins.tsv").map(lambda line: line.split("\t")).map(lambda list: (list[2], list[1]))
table2 = sc.textFile("little_pins.tsv").map(lambda line: line.split("\t")).map(lambda list: (list[2], list[1]))

graph = table1.join(table2).map(lambda (board, (pin1, pin2)): sorted((pin1, pin2))).filter(lambda pins : pins[0] != pins[1])\
.map(lambda pins: " ".join(pins)).distinct()

graph.saveAsTextFile("graph")

sc.stop()
