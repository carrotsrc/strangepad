OBJ=rso/SuFlac.rso rso/SuAlsa.rso
CATCH_SRC=catch_suflac.cpp catch_flacalsa.cpp
all: suite

suite: $(OBJ) suite.cpp $(CATCH_SRC)
	g++ -ggdb -std=c++11 -I$(STRANGEFW)/framework/include/ \
	-L$(STRANGEFW)/ -lstrangeio -ldl -o suite $(OBJ) \
	suite.cpp $(CATCH_SRC) -pthread  -lpthread
	
rso/SuFlac.rso: ../units/strange/SuFlac.cpp ../units/strange/SuFlac.hpp
	unitbuild rso/SuFlac -ggdb -lsndfile ../units/strange/SuFlac.cpp 

rso/SuAlsa.rso: ../units/strange/SuAlsa.cpp ../units/strange/SuAlsa.hpp
	unitbuild rso/SuAlsa -ggdb -lasound ../units/strange/SuAlsa.cpp 