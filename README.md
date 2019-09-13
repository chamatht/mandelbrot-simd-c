# mandelbrot-simd-c
C implementation of mandelbrot set with gcc vector extensions. Gcc and Clang support [GCC vector extensions](https://gcc.gnu.org/onlinedocs/gcc/Vector-Extensions.html).

Program will be compiled into simd instructions (AVX/AVX2 for x86, NEON for arm)

To compile: run
```
make
```
<img src="https://github.com/chamatht/mandelbrot-simd-c/raw/master/image.png" width="500">
