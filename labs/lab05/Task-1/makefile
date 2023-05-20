CC = gcc
CFLAGS = -O3 -march=native -ffast-math

%s: %s.c 
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf array_loop loop_invariants string_loop || true
