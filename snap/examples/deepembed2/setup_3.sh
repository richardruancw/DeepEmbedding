#get snap-python
wget http://snap.stanford.edu/snappy/release/snap-3.0.2-3.0-centos6.5-x64-py2.6.tar.gz
tar zxvf snap-3.0.2-3.0-centos6.5-x64-py2.6.tar.gz
cd snap-3.0.2-3.0-centos6.5-x64-py2.6
sudo python setup.py install
cd ..
sudo rm snap-3.0.2-3.0-centos6.5-x64-py2.6.tar.gz

# should be good to go