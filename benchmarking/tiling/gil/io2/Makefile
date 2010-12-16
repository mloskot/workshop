LIBS=-ltiff
INC=-I/home/mloskot/dev/boost/_svn/sandbox/gil
PRG=io2_tiff_image_tiles_test
SRC=$(PRG).cpp
CXX=g++

all: release200 release256 release512

debug:	debug200 debug256 debug512

debug200:	$(SRC)
	$(CXX) -g $(CXXFLAGS) $(CPPFLAGS) $(INC) $(LIBS) -o $(PRG) $(SRC)

release200:	$(SRC)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) $(CPPFLAGS) $(INC) $(LIBS) -o $(PRG) $(SRC)

clean:
	$(RM) *.o 
	$(RM) output/*.png
	$(RM) output/*.jpg