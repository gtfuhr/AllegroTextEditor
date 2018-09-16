
CXX = g++
CXXFLAGS = -g -std=c++11 -Wall 
LDFLAGS = -lallegro -lallegro_main \
    -lallegro_color -lallegro_font -lallegro_primitives -lallegro_image

all: principal

principal.o: principal.cpp geom.hpp
Tela.o: Tela.cpp Tela.hpp geom.hpp

principal: principal.o Tela.o 
	$(CXX) $(CXXFLAGS) -o $@  $^ $(LDFLAGS)

clean:
	rm -f principal *.o
