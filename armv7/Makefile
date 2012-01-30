CXX=g++

PREFIX=/usr/local
LIB_DIR=$(PREFIX)/lib
INCLUDE_DIR=$(PREFIX)/include/armv7

HEADERS=*.hpp

# Release build
CXXFLAGS_REL=-Wall -O3 -static
OBJ_REL=function.o
OUT_REL=libarmisa.a

# Debug and profiling build
CXXFLAGS_DBG=-Wall -O0 -g -static -coverage
OBJ_DBG=function-dbg.o
OUT_DBG=libarmisa-dbg.a


.PHONY:  all install install-rel install-dbg uninstall clean depend
all:     $(OUT_REL) $(OUT_DBG)
install: install-rel install-dbg


# Release build
$(OUT_REL): $(OBJ_REL)
	ar rcs $(OUT_REL) $(OBJ_REL)

function.o: function.cpp
	$(CXX) $(CXXFLAGS_REL) -c -o function.o function.cpp

install-rel: $(OUT_REL)
	cp $(OUT_REL) $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	cp $(HEADERS) $(INCLUDE_DIR)


# Debug and profiling build
$(OUT_DBG): $(OBJ_DBG)
	ar rcs $(OUT_DBG) $(OBJ_DBG)

function-dbg.o: function.cpp
	$(CXX) $(CXXFLAGS_DBG) -c -o function-dbg.o function.cpp


install-dbg: $(OUT_DBG)
	cp $(OUT_DBG) $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	cp $(HEADERS) $(INCLUDE_DIR)


# Uninstall and clean
uninstall:
	rm -rf $(INCLUDE_DIR)
	rm -f  $(LIB_DIR)/$(OUT_REL)
	rm -f  $(LIB_DIR)/$(OUT_DBG)

clean:
	rm -f $(OUT_REL) $(OUT_DBG) $(OBJ_REL) $(OBJ_DBG)
	rm -f .depend *.gcda *.gcno

# Dependencies
depend: .depend

.depend: *.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > .depend

-include .depend
