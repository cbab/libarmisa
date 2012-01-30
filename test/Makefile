CXX=g++
CXXFLAGS=-Wall -O0 -g -static -I.. --coverage
LDFLAGS=-L../armv7 -larmisa-dbg -lboost_unit_test_framework
OBJ=main.o
OUT=test
HEADERS=

.PHONY: all clean depend

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT) $(OBJ) .depend *.gcno *.gcda

depend: .depend

.depend: *.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > .depend

-include .depend
