#
#	configuration variables for the example

## Main application file
MAIN = deepembed
DEPH = $(EXSNAPADV)/n2v.h $(EXSNAPADV)/word2vec.h $(EXSNAPADV)/biasedrandomwalk.h $ deeputils.h samplegraph.h recoveredges.h linearinterpolation.h 

DEPCPP = $(EXSNAPADV)/n2v.cpp $(EXSNAPADV)/word2vec.cpp $(EXSNAPADV)/biasedrandomwalk.cpp $ deeputils.cpp samplegraph.cpp recoveredges.cpp linearinterpolation.cpp

CXXFLAGS += $(CXXOPENMP)
 

