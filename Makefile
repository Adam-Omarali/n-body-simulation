CXX := clang++
CPPFLAGS := -Iinclude -isystem include/raylib
CXXFLAGS := -std=c++20 -g -O0 -Wall -Wextra -Wpedantic
LDLIBS := lib/libraylib.a \
	-framework CoreVideo \
	-framework IOKit \
	-framework Cocoa \
	-framework QuartzCore \
	-framework OpenGL

.PHONY: default clean bench

default: main
	./build/main

main: src/main.cpp src/renderer.cpp src/conservation_monitor.cpp src/direct_gravity.cpp src/bhtree.cpp lib/libraylib.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp src/renderer.cpp src/conservation_monitor.cpp src/direct_gravity.cpp src/bhtree.cpp -o build/$@ $(LDLIBS)

BENCH_CXXFLAGS := -std=c++20 -g -O2 -Wall -Wextra -Wpedantic

bench: src/bench.cpp src/direct_gravity.cpp src/bhtree.cpp
	$(CXX) $(CPPFLAGS) $(BENCH_CXXFLAGS) src/bench.cpp src/direct_gravity.cpp src/bhtree.cpp -o build/$@

clean:
	rm -f main
