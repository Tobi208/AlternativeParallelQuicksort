CC = gcc
SOURCE = quicksort.c
EXE = quicksort
CFLAGS = -Wall -O3 -ffast-math -march=native -lpthread
N = 100000000             # input size
INPUT_DIR = input
INPUT = $(INPUT_DIR)/$(N)
T = 1                     # number of threads
S = 1                     # pivot strategy {1, 2, 3}

DIST = 0                  # distribution strategy {0, 1, 2, 3}
SEED = 0                  # rand seed, 0 = currennt time

DFLAGS = 
DEBUG = 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g
endif
CHECK_SORTED = 0
ifeq ($(CHECK_SORTED), 1)
    DFLAGS += -DCHECK_SORTED 
endif
CHECK_COMPLETE = 0
ifeq ($(CHECK_COMPLETE), 1)
    DFLAGS += -DCHECK_COMPLETE 
endif

$(EXE): $(SOURCE)
	$(CC) $(SOURCE) -o $(EXE) $(CFLAGS) $(DFLAGS)

run: $(EXE)
	./$(EXE) $(N) $(INPUT) $(T) $(S)

# @echo "N = $(N)"
# @echo "DIST,S,T,t"
time: $(EXE)
	@for S in 1 2 3 ; do \
		for T in 1 2 4 8 16 32 ; do \
			echo -n "$(DIST),$$S,$$T," ; \
			./$(EXE) $(N) $(INPUT) $$T $SS; \
			echo -n "$(DIST),$$S,$$T," ; \
			./$(EXE) $(N) $(INPUT) $$T $SS; \
			echo -n "$(DIST),$$S,$$T," ; \
			./$(EXE) $(N) $(INPUT) $$T $SS; \
		done \
	done

vecinfo: $(SOURCE)
	$(CC) $(SOURCE) -o $(EXE) $(CFLAGS) $(DFLAGS) -fopt-info-vec

generator: generator.c
	$(CC) generator.c -o generator $(CFLAGS)

generate: generator
	mkdir -p $(INPUT_DIR)
	./generator $(N) $(INPUT_DIR) $(DIST) $(SEED)

clean:
	rm -f $(EXE) generator
	rm -rf $(INPUT_DIR)
