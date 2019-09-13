CFLAGS=-march=native -O2 -ffast-math -fopenmp -flto
LIBS=-lpng

simd-mandelbrot: simd-mandelbrot.o 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm *.o simd-mandelbrot
