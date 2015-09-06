OBJ=rso/SuFlac.rso
all: suite

suite: $(OBJ) suite.cpp catch_suflac.hpp
	g++ -ggdb -std=c++11 -I$(STRANGEFW)/framework/include/ -L$(STRANGEFW)/ -lstrangeio -ldl -o suite $(OBJ) suite.cpp -pthread  -lpthread
	
rso/SuFlac.rso: ../units/strange/SuFlac.cpp ../units/strange/SuFlac.hpp
	unitbuild rso/SuFlac -ggdb -lsndfile ../units/strange/SuFlac.cpp 