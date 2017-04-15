#
#	configuration variables for the example

## Main application file
MAIN = deepembed
DEPH = $(EXSNAPADV)/n2v.h $(EXSNAPADV)/word2vec.h $(EXSNAPADV)/biasedrandomwalk.h $ deeputils.h recoveredges.h linearinterpolation.h samplen2v.h jumpword2vec.h

DEPCPP = $(EXSNAPADV)/n2v.cpp $(EXSNAPADV)/word2vec.cpp $(EXSNAPADV)/biasedrandomwalk.cpp $ deeputils.cpp recoveredges.cpp linearinterpolation.cpp samplen2v.cpp jumpword2vec.cpp

CXXFLAGS += $(CXXOPENMP)
 

