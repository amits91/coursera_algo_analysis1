#FLAGS := -g
#SPEEDUPFLAGS := -O3 -msse4.2 -ffast-math
#SPEEDUPFLAGS := -O3 -msse4.2 -ftree-vectorize -fno-rtti -fno-exceptions -fomit-frame-pointer -funroll-loops  -ftree-vectorizer-verbose=10 -ffast-math
SPEEDUPFLAGS := -O3 -msse4.2 -ftree-vectorize -fno-rtti -fno-exceptions -fomit-frame-pointer -funroll-loops  -ffast-math -minline-all-stringops -minline-stringops-dynamically -march=native
#SPEEDUPFLAGS := 
#GCCOPENMP:=  /u/prod/gnu/gcc/20100526/gcc-4.5.0-linux_x86_64/bin/gcc
GCCOPENMP:=  g++
F :=
N :=

all: clean compile_dbg run

noomp: compile run

compile: main.cpp
	@echo "-- Compiling"
	$(GCCOPENMP) $(FLAGS) $(SPEEDUPFLAGS) main.cpp -o run

compile_dbg: main.cpp
	@echo "-- Compiling DBG"
	$(GCCOPENMP) $(FLAGS) -g main.cpp -o run
compile_omp:
	@echo "-- Compiling"
	$(GCCOPENMP) -fopenmp $(FLAGS) $(SPEEDUPFLAGS) main.cpp -o run
run:
	@echo "-- Running program"
	./run $(F) $(N)
clean:
	rm -f run l log *dot*
dot:
	dot -Tjpeg g.dot -O
	dot -Tjpeg gRev.dot -O




.PHONY: compile run
