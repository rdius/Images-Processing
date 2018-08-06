
OB += \
./Trans_fourier.o 
	

%.o:
	g++  -O0 -g3 -Wall -c  Trans_fourier.cpp  `pkg-config --cflags --libs opencv`

all:$(OB)
	@echo 'Building target: $@'
	g++  -o "Trans_fourier" $(OB)  `pkg-config --cflags --libs opencv`
	@echo 'Finished building target: $@'


clean:
	-rm $(OB) Trans_fourier
