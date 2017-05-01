from pyspark.sql.functions import *
from pyspark.sql import SparkSession

spark = SparkSession.builder \
.master("local") \
.appName("Word Count") \
.config("spark.some.config.option", "some-value") \
.getOrCreate()

df1 = spark.read.csv("gs://pinterest-data/pins.tsv", header = False, sep = "\t")
df2 = spark.read.csv("gs://pinterest-data/little_pins.tsv", header = False, sep = "\t")

df1 = df1.select(['_c1','_c2'])
df2 = df1.select(['_c1','_c2'])

df1 = df1.withColumnRenamed("_c1","a_c1").withColumnRenamed("_c2","a_c2")
df2 = df2.withColumnRenamed("_c1","b_c1").withColumnRenamed("_c2","b_c2")

df3 = df1.join(df2, (df1.a_c2 == df2.b_c2) & (df1.a_c1 < df2.b_c1), "inner")

df4 = df3.groupBy(["a_c1","b_c1"]).count()
df4.write.csv("gs://pinterest-data/graph", header = False, sep = "\t")