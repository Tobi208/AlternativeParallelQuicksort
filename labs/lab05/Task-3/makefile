CC = gcc
CFLAGS = -O3 -march=native -ffast-math
LIBS = -lm

%s: %s.c 
	$(CC) $(CFLAGS) $(LIBS) -o $@ $< 

clean:
	rm -rf short-curciut bounds || true
