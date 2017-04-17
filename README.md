# DeepEmbedding
CS341-Minning Massive Data Sets Project

This is the repo for the project __Large Scale Deep Embeddings__.

## Required Softwares

* Stanford Network Analysis Platform

The __Large Scale Deep Embeddings__ is in the ``examples`` folder of the forked ``SNAP`` repo. Run the makefile in that folder will complie the source files and link necessary ``SNAP`` files.


If use ``Debian GNU/Linux 8 (jessie)`` VM provided by Google computing engine. Run:

```shell
sudo apt-get install git
sudo apt-get install make
sudo apt-get install build-essential
cd DeepEmbedding/snap/examples/deepembed/
make all
```


If use ``CentOS7``, run the following commands

```
sudo yum install git
sudo yum install make
sudo yum groupinstall 'Development Tools'
git clone https://github.com/richardruancw/DeepEmbedding.git
cd DeepEmbedding/snap/examples/deepembed/
make all
```
