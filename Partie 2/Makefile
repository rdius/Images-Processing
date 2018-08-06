
OB += \
fourier_filtres.o \

	

%.o:
	g++  -O0 -g3 -Wall -c  fourier_filtres.cpp  `pkg-config --cflags --libs opencv`

all:$(OB)
	@echo 'Building target: $@'
	g++  -o "fourier_filtres" $(OB)  `pkg-config --cflags --libs opencv`
	@echo 'Finished building target: $@'

clear:
	rm -rf *.o

mproper: clean
	rm -rf $(OB) fourier_filtres
