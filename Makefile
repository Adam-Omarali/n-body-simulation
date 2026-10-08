CXX := clang++
CPPFLAGS := -Isrc/include
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic
LDLIBS := lib/libraylib.a \
	-framework CoreVideo \
	-framework IOKit \
	-framework Cocoa \
	-framework QuartzCore \
	-framework OpenGL

.PHONY: default clean

default: main
	./build/main

main: src/main.cpp lib/libraylib.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp -o build/$@ $(LDLIBS)

clean:
	rm -f main
