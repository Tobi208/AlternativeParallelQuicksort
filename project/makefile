N = 1000000
INPUT_DIR = input
T = 1

quicksort: quicksort.c
	gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -fopenmp

vecinfo: quicksort.c
	gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -fopenmp -fopt-info-vec

generator: generator.c
	gcc generator.c -o generator -Wall -O3 -ffast-math -march=native

generate: generator
	mkdir -p $(INPUT_DIR)
	./generator $(N) $(INPUT_DIR)

run: quicksort
	./quicksort $(N) $(INPUT_DIR)/$(N) $(T)

clean:
	rm -f quicksort generator
	rm -rf $(INPUT_DIR)
