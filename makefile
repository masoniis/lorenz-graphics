# Executable Name
EXE=hw2

# Object files
OBJ=main.o state.o lorenz.o

# target
all: $(EXE)

# Platform-specific configuration
#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Implicit rule for compiling C files
.c.o:
	gcc -c $(CFLG) $<

# Link the executable
$(EXE): $(OBJ)
	gcc $(CFLG) -o $@ $^ $(LIBS)

# Clean up build files
clean:
	$(CLEAN)
