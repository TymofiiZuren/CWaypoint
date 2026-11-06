CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Isrc/learning
.PHONY: test demo
test:
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/lesson_test.cpp src/learning/lesson.cpp -o build/lesson-test
	./build/lesson-test
	clang -std=c11 -Wall -Wextra -Wpedantic examples/variables.c -o build/variables
	./build/variables > build/actual.txt
	printf '5\n4\n' > build/expected.txt
	diff -u build/expected.txt build/actual.txt
demo:
	mkdir -p build
	$(CXX) $(CXXFLAGS) examples/lesson_demo.cpp src/learning/lesson.cpp -o build/lesson-demo
	./build/lesson-demo
