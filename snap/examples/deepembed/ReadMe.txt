========================================================================
    DeepEmbedding: Super Large Scale Deepembedding 
========================================================================

The application implements super large scale embedding learning algorithm
for large networks with number of nodes up to 1 billion. This algorithm is 
still under development.

Author(without order): Bowen Yao, Chuanwei Ruan, Yue Liu 

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:
InFile - Input graph path
OutFile - Output graph path
Option - 
        Algorithm option. 1: recover edges, 2: sampledn2v, any other number: origin node2vec.
Dimensions - Number of dimensions. Default is 128.
WalkLen - Length of walk per source. Default is 80
NumWalks - Number of walks per source. Default is 10
WinSize - Context size for optimization. Default is 10
ShrinkFactor - Shrink factor of nodes number. Default is 100
Iter - Number of epochs in SGD. Default is 1
ParamP - Return hyperparameter. Default is 1
ParamQ - Inout hyperparameter. Default is 1
Verbose - "-v", Verbose output
Directed - "-dr" Graph is directed
  Weighted - "-w" Graph is weighted
/////////////////////////////////////////////////////////////////////////////
Usage:
./deepembed -i:graph/facebook_combined.edgelist -o:karate.emb -l:10 -d:24 -p:0.3  -q:0.5 -v -s:1.5 -option:1
