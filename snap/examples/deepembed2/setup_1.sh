#get git, make and source code, Development Tools, screen, wget
sudo yum install git
sudo yum install make
sudo yum groupinstall 'Development Tools'
sudo yum install wget
yum -y install screen
git clone https://github.com/richardruancw/DeepEmbedding.git
cd DeepEmbedding/snap/examples/deepembed2/
make all

#get graphs we need
cd graph
gsutil cp gs://deepembed_exp_data/*
cd ~

#get anaconda, do accordingly on the shell
wget https://repo.continuum.io/archive/Anaconda2-4.3.1-Linux-x86_64.sh
bash Anaconda2-4.3.1-Linux-x86_64.sh




