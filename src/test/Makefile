OBJ=rso/SuFlac.rso rso/SuAlsa.rso
all: suite

suite: $(OBJ) suite.cpp catch_suflac.hpp
	g++ -ggdb -std=c++11 -I$(STRANGEFW)/framework/include/ -L$(STRANGEFW)/ -lstrangeio -ldl -o suite $(OBJ) suite.cpp -pthread  -lpthread
	
rso/SuFlac.rso: ../units/strange/SuFlac.cpp ../units/strange/SuFlac.hpp
	unitbuild rso/SuFlac -ggdb -lsndfile ../units/strange/SuFlac.cpp 

rso/SuAlsa.rso: ../units/strange/SuAlsa.cpp ../units/strange/SuAlsa.hpp
	unitbuild rso/SuAlsa -ggdb -lsndfile ../units/strange/SuAlsa.cpp 