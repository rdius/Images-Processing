
OB += \
./profil.o
	

%.o:
	g++  -O0 -g3 -Wall -c  profil.cpp  `pkg-config --cflags --libs opencv`

all:$(OB)
	@echo 'Building target: $@'
	g++  -o "profil" $(OB)  `pkg-config --cflags --libs opencv`
	@echo 'Finished building target: $@'


clean:
	-rm $(OB) profil


