DESTDIR=
PREFIX=/usr/local
CXXFLAGS = -O1 -g \
	-fsanitize=address \
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls

all: dylibbundler
HEADERS =  src/Settings.h src/Utils.h src/Dependency.h src/DylibBundler.h

Settings.o: src/Settings.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -I./src ./src/Settings.cpp -o ./Settings.o

DylibBundler.o: src/DylibBundler.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -I./src ./src/DylibBundler.cpp -o ./DylibBundler.o

Dependency.o: src/Dependency.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -I./src ./src/Dependency.cpp -o ./Dependency.o

main.o: src/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -I./src ./src/main.cpp -o ./main.o

Utils.o: src/Utils.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -I./src ./src/Utils.cpp -o ./Utils.o

dylibbundler: Settings.o DylibBundler.o Dependency.o main.o Utils.o
	$(CXX) $(CXXFLAGS) -o ./dylibbundler ./Settings.o ./DylibBundler.o ./Dependency.o ./main.o ./Utils.o

clean:
	rm -f *.o
	rm -f ./dylibbundler

install: dylibbundler
	cp ./dylibbundler $(DESTDIR)$(PREFIX)/bin/dylibbundler
	chmod 775 $(DESTDIR)$(PREFIX)/bin/dylibbundler

tests: link_boost_filesystem dylibbundler
	mkdir -p test_output/test1/Contents/MacOS
	cp link_boost_filesystem test_output/test1/Contents/MacOS
	echo | ./dylibbundler -b -x test_output/test1/Contents/MacOS/link_boost_filesystem \
		-p @executable_path/../libs/ \
		-cd -od -d test_output/test1/Contents/libs

link_boost_filesystem: tests/link_boost_filesystem.cpp
	$(CXX) $(CXXFLAGS) -I./src -lboost_filesystem -lboost_system "$<" -o "$@"

.PHONY: all clean install
