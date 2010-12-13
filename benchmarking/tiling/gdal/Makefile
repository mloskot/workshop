ifeq ($(JPG), yes)
	CPPFLAGS+=-DTEST_OUTPUT_JPG=1
endif

GDAL_LIBS=$(shell gdal-config --libs)
GDAL_DEPS=$(shell gdal-config --dep-libs)
LIBS=$(GDAL_LIBS) $(GDAL_DEPS)

CXX=g++

all: release200 release256 release512

debug:	debug200 debug256 debug512

debug200:	gdal_image_tiles_test.cpp
	$(CXX) -g $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_200 -o gdal_image_tiles_test_200_d gdal_image_tiles_test.cpp $(LIBS)

release200:	gdal_image_tiles_test.cpp
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_200 -o gdal_image_tiles_test_200 gdal_image_tiles_test.cpp $(LIBS)

debug256:	gdal_image_tiles_test.cpp
	$(CXX) -g $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_256 -o gdal_image_tiles_test_256_d gdal_image_tiles_test.cpp $(LIBS)

release256:	gdal_image_tiles_test.cpp
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_256 -o gdal_image_tiles_test_256 gdal_image_tiles_test.cpp $(LIBS)

debug512:	gdal_image_tiles_test.cpp
	$(CXX) -g $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_512 -o gdal_image_tiles_test_512_d gdal_image_tiles_test.cpp $(LIBS)

release512:	gdal_image_tiles_test.cpp
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) $(CPPFLAGS) -DTEST_TILE_512 -o gdal_image_tiles_test_512 gdal_image_tiles_test.cpp $(LIBS)

clean:
	$(RM) *.o *_200* *_256* *_512*
	$(RM) output/*.png
	$(RM) output/*.jpg