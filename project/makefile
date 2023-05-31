N = 100000000
INPUT_DIR = input
T = 1

quicksort: quicksort.c
	gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -lpthread

vecinfo: quicksort.c
	gcc quicksort.c -o quicksort -Wall -O3 -ffast-math -march=native -lpthread -fopt-info-vec

generator: generator.c
	gcc generator.c -o generator -Wall -O3 -ffast-math -march=native

generate: generator
	mkdir -p $(INPUT_DIR)
	./generator $(N) $(INPUT_DIR)

run: quicksort
	./quicksort $(N) $(INPUT_DIR)/$(N) $(T)

time: quicksort
	@echo "N = $(N)"
	@echo "T,t"
	@for T in 1 2 4 8 16 32 ; do \
		echo -n "$$T," ; \
		./quicksort $(N) $(INPUT_DIR)/$(N) $$T ; \
	done

clean:
	rm -f quicksort generator
	rm -rf $(INPUT_DIR)
