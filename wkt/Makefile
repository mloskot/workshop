CPP = g++
LDFLAGS = -L/usr/local/lib -lboost_unit_test_framework-gcc
CFLAGS = -g -I/usr/local/include/boost-1_34 -I../src $(CPPFLAGS)
PROGS = ogc_wkt_grammar_test ogc_wkt_validator_test error_handling_test

all: $(PROGS)

ogc_wkt_grammar_test: ogc_wkt_grammar_test.cpp
	$(CPP) $(CFLAGS) $< -o $@ $(LDFLAGS)

ogc_wkt_validator_test: ogc_wkt_validator_test.cpp
	$(CPP) $(CFLAGS) $< -o $@ $(LDFLAGS)

error_handling_test: error_handling_test.cpp
	$(CPP) $(CFLAGS) $< -o $@ $(LDFLAGS)

test: all
	$(PROGS)

clean:
	$(RM) $(PROGS) *.o *.a *~ *.out

