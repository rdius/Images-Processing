
OB += \
./contraste.o\

	

%.o:
	g++  -O0 -g3 -Wall -c contraste.cpp `pkg-config --cflags --libs opencv`

all:$(OB)
	@echo 'Building target: $@'
	g++  -o "contraste" $(OB)  `pkg-config --cflags --libs opencv`
	@echo 'Finished building target: $@'

clear:
	-rm $(OB) profilmproper :

mproper: clean
	-rm -rf $(OB) fonction
