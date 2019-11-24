build:
	@flex -o main.yy.cpp main.l
	@bison -o main.tab.cpp -d main.y
	@g++ --std=c++14 `llvm-config --cxxflags --ldflags --libs --libfiles --system-libs` \
		-o main.out main.cpp main.tab.cpp main.yy.cpp mast.cpp -ll

clean:
	@rm -f basic.out basic.o main.out main.tab.cpp main.tab.hpp main.yy.cpp
