CXX = g++
CXXFLAGS = -O2 -Wall 
OPENCV	= `pkg-config --cflags --libs opencv`
EXEC = graphoScan

all: $(EXEC)

graphoScan : graphoScan.o tracking.o roiSelector.o
	$(CXX) -o $@ $^ 

roiSelectoir.o :
	g++ roiSelector.hpp `pkg-config --cflags --libs opencv`

%.o : %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(OPENCV)

  
clean:
	rm -rf *.o

mrproper : clean
	rm -rf $(EXEC)
