# DeepEmbedding
CS341-Minning Massive Data Sets Project

This is the repo for the project __Large Scale Deep Embeddings__.

## Required Softwares

* Stanford Network Analysis Platform

Run following codes to install snap, which implements the ``node2vec`` algorithm.

If use ``Debian GNU/Linux 8 (jessie)`` VM provided by Google computing engine. Run:

```shell
sudo apt-get install git
sudo apt-get install make
sudo apt-get install build-essential
```

Move to snap folder, and complie using provided makefiles

```shell
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
