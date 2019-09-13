#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include <omp.h>

#include "pngutil.h"

typedef int32_t i32x8 __attribute__ ((vector_size (32)));
typedef float f32x8 __attribute__ ((vector_size (32)));

#define v8same(x) {(x),(x),(x),(x),(x),(x),(x),(x)}
#define v8_alleq(x,y) memcmp((&x),(&y), sizeof(x))==0
#define mask_if_else(mask, vtrue, vfalse) ( ((i32x8)(mask)&(i32x8)(vtrue)) | ((i32x8)(~mask)&(i32x8)(vfalse)) )

#define IMG_WIDTH 2048
#define IMG_HEIGHT 2048
#define MAX_ITERATIONS 32

int writeImage(char* filename, int width, int height, unsigned char* buffer, char* title);

int main(void)
{   
    unsigned char* image = (unsigned char*) malloc(IMG_WIDTH*IMG_HEIGHT*3*sizeof(*image));

    const float x_cmin = -2.0f;
    const float x_cmax = 1.0f;
    const float y_cmin = -1.5f;
    const float y_cmax = 1.5f;
    
    const float scale_x = (x_cmax - x_cmin) / IMG_WIDTH;
    const float scale_y = (y_cmax - y_cmin) / IMG_HEIGHT;
    const int iter_char_scale = 255/MAX_ITERATIONS;

    i32x8 vfalse = v8same(0);
    
    #pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < IMG_HEIGHT; ++y){
        for (int x = 0; x < IMG_WIDTH; x+=8){
            f32x8 vx = {x+0.0f, x+1.0f, x+2.0f, x+3.0f, 
                        x+4.0f, x+5.0f, x+6.0f, x+7.0f};
            f32x8 cx = v8same(0.0f), cy = v8same(0.0f);
            cx += x_cmin + (vx*scale_x);
            cy += y_cmax - (y*scale_y);
            f32x8 zx = cx, zy = cy;
             
            i32x8 ret = v8same(1);
            i32x8 isInside = v8same(-1); //-1 == true

            for (int q = 0; q < MAX_ITERATIONS; ++q){
                f32x8 zx2 = zx*zx;
                f32x8 zy2 = zy*zy;
                f32x8 zxzy = zx*zy;         
                zx = (f32x8)mask_if_else(isInside, (zx2 - zy2 + cx), zx);
                zy = (f32x8)mask_if_else(isInside, (zxzy + zxzy + cy), zy);

                zx2 = zx*zx;
                zy2 = zy*zy;
                isInside = (zx2+ zy2) <= 4.0f;
                ret = mask_if_else(isInside, ret+1, ret);
                if (v8_alleq(isInside, vfalse)) break;
            }

            for (int i = 0; i < 8; ++i)
            {
                unsigned char j = (unsigned char)((ret[i]*iter_char_scale)%255);
                int k = (y*IMG_WIDTH + x + i)*3;
                image[k+0] = 255*x/IMG_WIDTH;
                image[k+1] = j;
                image[k+2] = 255*y/IMG_HEIGHT;
                //image[k+3] = 255;
            }
        }   
    }
    
    int ret = writeImage("image.png", IMG_WIDTH, IMG_HEIGHT, image, "Mandelbrot");
    free(image);
    
    return ret;
}
