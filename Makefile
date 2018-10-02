CC=g++

CFLAGS=-Wall -std=c++14

# We need to include some source files from unity into the build
UNITY=Unity
UNITY_FILES=$(UNITY)/src/unity.c 

# Source files
SRCS=$(UNITY_FILES) $(wildcard src/*.c) $(wildcard lib/FQueue/*.cpp)

# Add additional tests here
TESTS = TestFQueue

# Test sources and runners. Don't touch the following lines!
TEST_SRCS=$(addprefix test/,$(addsuffix(.cpp, $(TESTS))))
RUNNERS=$(addprefix test/,$(addsuffix(_Runner.c, $(TESTS))))
TARGETS=$(addprefix run_, $(TESTS))

# Include directories
INCL=-Isrc -Ilib/FQueue -I$(UNITY)/src 


.PHONY: all clean run_tests build coverflags

default all: Unity clean run_tests

Unity:
	git submodule init
	git submodule update

test/%_Runner.c: test/%.cpp 
	ruby $(UNITY)/auto/generate_test_runner.rb $< $@

run_%: $(SRCS) test/%.cpp test/%_Runner.c
	$(CC) $(CFLAGS) $(INCL) $(SRCS) test/$*.cpp test/$*_Runner.c -o $@

run_tests: $(TARGETS)
	@for TEST in $(TARGETS); do		\
		echo Running $${TEST};		\
		./$${TEST};			\
	done					

build:
	pio run

coverflags:
	$(eval CFLAGS += -O0 --coverage)

coverage: clean coverflags run_tests
	mkdir -p cover report
	rm -f Test*.gc* unity.gc*
	mv *.gc* cover
	lcov -c -d cover -o coverage.info
	genhtml -o report coverage.info
	firefox report/index.html

clean:
	rm -rf $(TARGETS) $(RUNNERS) *~ src/*~ test/*~ cover report coverage.info

