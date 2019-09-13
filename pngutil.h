#ifndef __PNGUTIL__
#include <stdlib.h>
#include <string.h>
#include <png.h>

int writeImage(char* filename, int width, int height, unsigned char* buffer, char* title)
{
    int code = 0;
    FILE* fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    size_t row_size = 0;

    fp = fopen(filename, "wb");
    if (fp == NULL) {
       fprintf(stderr, "Could not open file %s for writing\n", filename);
       code = 1;
       goto finalise;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
       fprintf(stderr, "Could not allocate write struct\n");
       code = 1;
       goto finalise;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
       fprintf(stderr, "Error during png creation\n");
       code = 1;
       goto finalise;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, 
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    if (title != NULL) {
       png_text title_text;
       title_text.compression = PNG_TEXT_COMPRESSION_NONE;
       title_text.key = "Title";
       title_text.text = title;
       png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    row_size = 3*width*sizeof(png_byte);
    row = (png_bytep) malloc(row_size);
    for(int y=0; y<height; y++) {
        memcpy(row, &buffer[y*width*3], row_size);
        png_write_row(png_ptr, row);
    }
    
    png_write_end(png_ptr, info_ptr);

finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (info_ptr != NULL) free(info_ptr);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}

#endif