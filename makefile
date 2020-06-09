ARTOOLKITDIR = ./ARToolKit
INC_DIR = $(ARTOOLKITDIR)/include
LIB_DIR = $(ARTOOLKITDIR)/lib

LIBS = -lARgsub -lARvideo -lAR -lGL -lGLU -lglut -lm
NAMEEXEC = HolaMundo

all: $(NAMEEXEC)

$(NAMEEXEC): $(NAMEEXEC).c
	cc -I $(INC_DIR) -o $(NAMEEXEC) $(NAMEEXEC).c -L $(LIB_DIR) $(LIBS)
clean:
	rm -f *.o $(NAMEEXEC) *~ *.*~
