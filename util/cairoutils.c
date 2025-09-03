/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <cairo.h>
#include <png.h>
#include <jpeglib.h>
#include <zlib.h>

#include "os-features.h"

// 简单的PPM格式定义，不依赖netpbm库
typedef struct {
    unsigned char r, g, b;
} simple_pixel;

#include "ioutils.h"
#include "cairoutils.h"
#include "errors.h"

enum imgformat {
    PPM,
    PNG,
    JPEG,
};
typedef enum imgformat imgformat;

struct mycolor {
    const char* name;
    float r,g,b;
};
typedef struct mycolor mycolor;

static mycolor mycolors[] = {
    { "darkred",      0.5, 0.0, 0.0 },
    { "red",          1.0, 0.0, 0.0 },
    { "darkgreen",    0.0, 0.5, 0.0 },
    { "green",        0.0, 1.0, 0.0 },
    { "blue",         0.0, 0.0, 1.0 },
    { "verydarkblue", 0.0, 0.0, 0.2 },
    { "white",        1.0, 1.0, 1.0 },
    { "black",        0.0, 0.0, 0.0 },
    { "cyan",         0.0, 1.0, 1.0 },
    { "magenta",      1.0, 0.0, 1.0 },
    { "yellow",       1.0, 1.0, 0.0 },
    { "brightred",    1.0, 0.0, 0.2 },
    { "skyblue",      0.0, 0.5, 1.0 },
    { "orange",       1.0, 0.5, 0.0 },
    { "gray",         0.5, 0.5, 0.5 },
    { "darkgray",     0.25, 0.25, 0.25 },
};
static const int nmycolors = sizeof(mycolors)/sizeof(mycolor);

cairo_status_t cairoutils_file_write_func(void *closure,
                                          const unsigned char *data,
                                          unsigned int length) {
    FILE* fid = closure;
    if (fwrite(data, 1, length, fid) != length) {
        SYSERROR("Failed to write cairo data");
        return CAIRO_STATUS_WRITE_ERROR;
    }
    return CAIRO_STATUS_SUCCESS;
}

const char* cairoutils_get_color_name(int i) {
    if ((i < 0)  || (i >= nmycolors))
        return NULL;
    return mycolors[i].name;
}

int cairoutils_surface_status_errors(cairo_surface_t* surf) {
    int st = cairo_surface_status(surf);
    switch (st) {
    case CAIRO_STATUS_SUCCESS:
        return 0;
    case CAIRO_STATUS_NULL_POINTER:
        ERROR("Cairo null pointer");
        break;
    case CAIRO_STATUS_NO_MEMORY:
        ERROR("Cairo no memory");
        break;
    case CAIRO_STATUS_READ_ERROR:
        ERROR("Cairo read error");
        break;
    case CAIRO_STATUS_INVALID_CONTENT:
        ERROR("Cairo invalid content");
        break;
    case CAIRO_STATUS_INVALID_FORMAT:
        ERROR("Cairo invalid format");
        break;
    case CAIRO_STATUS_INVALID_VISUAL:
        ERROR("Cairo invalid visual");
        break;
    }
    return -1;
}

int cairoutils_cairo_status_errors(cairo_t* c) {
    cairo_status_t st = cairo_status(c);
    if (st == CAIRO_STATUS_SUCCESS)
        return 0;
    ERROR("Cairo: %s", cairo_status_to_string(st));
    return -1;
}

void cairoutils_draw_path(cairo_t* c, const double* xy, int N) {
    int i;
    for (i=0; i<N; i++) {
        double px, py;
        px = xy[2*i+0];
        py = xy[2*i+1];
        if (i == 0)
            cairo_move_to(c, px, py);
        else
            cairo_line_to(c, px, py);
    }
}

static int hexval(char c) {
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'A') && (c <= 'F'))
        return 0xA + (c - 'A');
    if ((c >= 'a') && (c <= 'f'))
        return 0xa + (c - 'a');
    return 0;
}

int cairoutils_parse_color(const char* color, float* r, float* g, float* b) {
    int i;
    for (i=0; i<nmycolors; i++) {
        if (!strcmp(color, mycolors[i].name)) {
            *r = mycolors[i].r;
            *g = mycolors[i].g;
            *b = mycolors[i].b;
            return 0;
        }
    }
    if (strlen(color) == 6) {
        *r = (float)(hexval(color[0]) * 0x10 + hexval(color[1])) / 255.0;
        *g = (float)(hexval(color[2]) * 0x10 + hexval(color[3])) / 255.0;
        *b = (float)(hexval(color[4]) * 0x10 + hexval(color[5])) / 255.0;
        return 0;
    }
    return -1;
}

int cairoutils_parse_rgba(const char* str, float* r, float* g, float* b, float* a) {
    sl* words = sl_split(NULL, str, " ");
    char* endp;
    char* s;
    if (!((sl_size(words) == 3) || (sl_size(words) == 4))) {
        sl_free2(words);
        return -1;
    }
    assert(r);
    assert(g);
    assert(b);
    s = sl_get(words, 0);
    *r = strtof(s, &endp);
    if (endp == s) goto bailout;
    s = sl_get(words, 1);
    *g = strtof(s, &endp);
    if (endp == s) goto bailout;
    s = sl_get(words, 2);
    *b = strtof(s, &endp);
    if (endp == s) goto bailout;

    if ((sl_size(words) == 4) && a) {
        s = sl_get(words, 3);
        *a = strtof(s, &endp);
        if (endp == s) goto bailout;
    }
    sl_free2(words);
    return 0;

 bailout:
    sl_free2(words);
    return -1;
}

struct mymarker {
    const char* name;
    void (*drawit)(cairo_t* cairo, double x, double y, double rad, const char* name);
};
typedef struct mymarker mymarker;

static void drawcircle(cairo_t* cairo, double x, double y, double rad, const char* name) {
    cairo_move_to(cairo, x+rad, y);
    cairo_arc(cairo, x, y, rad, 0.0, 2.0*M_PI);
}

static void drawcrosshair(cairo_t* cairo, double x, double y, double rad, const char* name) {
    double in = 0.5;
    double out = 1.5;
    cairo_move_to(cairo, x - rad*out, y);
    cairo_line_to(cairo, x - rad*in,  y);
    cairo_move_to(cairo, x + rad*out, y);
    cairo_line_to(cairo, x + rad*in,  y);
    cairo_move_to(cairo, x, y + rad*out);
    cairo_line_to(cairo, x, y + rad*in );
    cairo_move_to(cairo, x, y - rad*out);
    cairo_line_to(cairo, x, y - rad*in );
}
static void drawsquare(cairo_t* cairo, double x, double y, double rad, const char* name) {
    cairo_move_to(cairo, x - rad, y - rad);
    cairo_line_to(cairo, x - rad, y + rad);
    cairo_line_to(cairo, x + rad, y + rad);
    cairo_line_to(cairo, x + rad, y - rad);
    cairo_line_to(cairo, x - rad, y - rad);
}
static void drawdiamond(cairo_t* cairo, double x, double y, double rad, const char* name) {
    cairo_move_to(cairo, x - rad, y);
    cairo_line_to(cairo, x, y + rad);
    cairo_line_to(cairo, x + rad, y);
    cairo_line_to(cairo, x, y - rad);
    cairo_line_to(cairo, x - rad, y);
}
static void drawX(cairo_t* cairo, double x, double y, double rad, const char* name) {
    cairo_move_to(cairo, x - rad, y - rad);
    cairo_line_to(cairo, x + rad, y + rad);
    cairo_move_to(cairo, x + rad, y - rad);
    cairo_line_to(cairo, x - rad, y + rad);
}
static void drawXcrosshair(cairo_t* cairo, double x, double y, double rad, const char* name) {
    double in = 0.3; //0.5 / sqrt(2.0);
    double out = 1.2; //1.5 / sqrt(2.0);
    cairo_move_to(cairo, x - rad*out, y - rad*out);
    cairo_line_to(cairo, x - rad*in,  y - rad*in );
    cairo_move_to(cairo, x + rad*out, y + rad*out);
    cairo_line_to(cairo, x + rad*in,  y + rad*in );
    cairo_move_to(cairo, x - rad*out, y + rad*out);
    cairo_line_to(cairo, x - rad*in,  y + rad*in );
    cairo_move_to(cairo, x + rad*out, y - rad*out);
    cairo_line_to(cairo, x + rad*in,  y - rad*in );
    //drawcircle(cairo, x, y, rad*0.5, "circle");
}



static mymarker mymarkers[] = {
    { "circle",    drawcircle },
    { "crosshair", drawcrosshair },
    { "square",    drawsquare },
    { "diamond",   drawdiamond },
    { "X",   drawX },
    { "Xcrosshair",drawXcrosshair },
};
static const int nmymarkers = sizeof(mymarkers)/sizeof(mymarker);

const char* cairoutils_get_marker_name(int i) {
    if ((i < 0)  || (i >= nmymarkers))
        return NULL;
    return mymarkers[i].name;
}

int cairoutils_parse_marker(const char* name) {
    int i;
    for (i=0; i<nmymarkers; i++)
        if (!strcmp(name, mymarkers[i].name))
            return i;
    return -1;
}

void cairoutils_draw_marker(cairo_t* cairo, int id,
                            double x, double y, double radius) {
    if ((id < 0)  || (id >= nmymarkers))
        return;
    mymarkers[id].drawit(cairo, x, y, radius, mymarkers[id].name);
    //cairo_stroke(cairo);
}

void cairoutils_print_marker_names(const char* prefix) {
    int i;
    for (i=0;; i++) {
        const char* marker = cairoutils_get_marker_name(i);
        if (!marker) break;
        if (prefix)
            printf("%s", prefix);
        printf("%s", marker);
    }
}

void cairoutils_print_color_names(const char* prefix) {
    int i;
    for (i=0;; i++) {
        const char* color = cairoutils_get_color_name(i);
        if (!color) break;
        if (prefix)
            printf("%s", prefix);
        printf("%s", color);
    }
}

unsigned char* cairoutils_read_jpeg(const char* fn, int* pW, int* pH) {
    FILE* fid;
    unsigned char* img;
    if (!strcmp(fn, "-")) {
        return cairoutils_read_jpeg_stream(stdin, pW, pH);
    }
    fid = fopen(fn, "rb");
    if (!fid) {
        fprintf(stderr, "Failed to open file %s\n", fn);
        return NULL;
    }
    img = cairoutils_read_jpeg_stream(fid, pW, pH);
    fclose(fid);
    return img;
}

unsigned char* cairoutils_read_png(const char* fn, int* pW, int *pH) {
    FILE* fid;
    unsigned char* img;
    fid = fopen(fn, "rb");
    if (!fid) {
        fprintf(stderr, "Failed to open file %s\n", fn);
        return NULL;
    }
    img = cairoutils_read_png_stream(fid, pW, pH);
    fclose(fid);
    return img;
}

static void user_error_fn(png_structp png_ptr, png_const_charp error_msg) {
    fprintf(stderr, "PNG error: %s\n", error_msg);
}
static void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg) {
    fprintf(stderr, "PNG warning: %s\n", warning_msg);
}

unsigned char* cairoutils_read_png_stream(FILE* fid, int* pW, int *pH) {
    png_structp ping;
    png_infop info;
    png_uint_32 W, H;
    unsigned char* outimg;
    png_bytepp rows;
    int j;
    int bitdepth, color_type, interlace;

    ping = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                  user_error_fn, user_warning_fn);
    if (!ping)
        return NULL;
    info = png_create_info_struct(ping);
    if (!info) {
        png_destroy_read_struct(&ping, NULL, NULL);
        return NULL;
    }

    png_init_io(ping, fid);
    png_read_info(ping, info);
    png_get_IHDR(ping, info, &W, &H, &bitdepth, &color_type,
                 &interlace, NULL, NULL);

    // see cairo's cairo-png.c
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(ping);
    if (color_type == PNG_COLOR_TYPE_GRAY && bitdepth < 8)
        png_set_expand(ping);
    if (png_get_valid(ping, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(ping);
    if (bitdepth == 16)
        png_set_strip_16(ping);
    if (bitdepth < 8)
        png_set_packing(ping);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(ping);
    if (interlace != PNG_INTERLACE_NONE)
        png_set_interlace_handling(ping);
    png_set_filler(ping, 0xff, PNG_FILLER_AFTER);
    png_read_update_info(ping, info);

    outimg = malloc(4 * W * H);
    rows = malloc(H * sizeof(png_bytep));
    if (!outimg || !rows) {
        free(outimg);
        png_destroy_read_struct(&ping, &info, NULL);
        return NULL;
    }
    for (j=0; j<H; j++)
        rows[j] = outimg + j*4*W;

    png_read_image(ping, rows);
    png_read_end (ping, info);

    png_destroy_read_struct(&ping, &info, NULL);
    free(rows);

    if (pW) *pW = W;
    if (pH) *pH = H;

    return outimg;
}

unsigned char* cairoutils_read_jpeg_stream(FILE* fid, int* pW, int* pH) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPLE* buffer;
    int row_stride;
    unsigned char* outimg;
    int W, H;
    int i, j;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fid);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = malloc(row_stride * sizeof(JSAMPLE));
    W = cinfo.output_width;
    H = cinfo.output_height;
    outimg = malloc(4 * W * H);
    for (j=0; j<H; j++) {
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        for (i=0; i<W; i++) {
            if (cinfo.output_components == 3) {
                outimg[4 * (j*W + i) + 0] = buffer[3*i + 0];
                outimg[4 * (j*W + i) + 1] = buffer[3*i + 1];
                outimg[4 * (j*W + i) + 2] = buffer[3*i + 2];
                outimg[4 * (j*W + i) + 3] = 255;
            } else if (cinfo.output_components == 1) {
                outimg[4 * (j*W + i) + 0] = buffer[i];
                outimg[4 * (j*W + i) + 1] = buffer[i];
                outimg[4 * (j*W + i) + 2] = buffer[i];
                outimg[4 * (j*W + i) + 3] = 255;
            }
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(buffer);

    if (pW) *pW = W;
    if (pH) *pH = H;

    return outimg;
}

static int streamout(FILE* fout, unsigned char* img, int W, int H, int format) {
    if (format == PPM) {
        // PPM...
        int i;
        fprintf(fout, "P6 %i %i %i\n", W, H, 255);
        for (i=0; i<(H*W); i++) {
            unsigned char* pix = img + 4*i;
            if (fwrite(pix, 1, 3, fout) != 3) {
                fprintf(stderr, "Failed to write pixels for PPM output: %s\n", strerror(errno));
                return -1;
            }
        }
    } else if (format == PNG) {
        // fires an ALPHA png out to fout
        png_bytepp image_rows;
        png_structp png_ptr;
        png_infop png_info;
        int n;

        image_rows = malloc(sizeof(png_bytep)*H);
        for (n = 0; n < H; n++)
            image_rows[n] = img + n*4*W;
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_info = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fout);
        png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
        png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
        png_set_IHDR(png_ptr, png_info, W, H, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(png_ptr, png_info);
        png_write_image(png_ptr, image_rows);
        png_write_end(png_ptr, png_info);
        free(image_rows);
        png_destroy_write_struct(&png_ptr, &png_info);
    } else if (format == JPEG) {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        JSAMPLE* buffer;
        int r;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, fout);
        cinfo.image_width = W;
        cinfo.image_height = H;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
        jpeg_set_colorspace(&cinfo, JCS_RGB);
        jpeg_simple_progression(&cinfo);
        jpeg_set_linear_quality(&cinfo, 70, FALSE);
        jpeg_start_compress(&cinfo, TRUE);
        buffer = malloc(W * 3);
        for (r=0; r<H; r++) {
            int i;
            for (i=0; i<W; i++) {
                buffer[i*3 + 0] = img[(r*W + i)*4 + 0];
                buffer[i*3 + 1] = img[(r*W + i)*4 + 1];
                buffer[i*3 + 2] = img[(r*W + i)*4 + 2];
            }
            jpeg_write_scanlines(&cinfo, &buffer, 1);
        }
        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        free(buffer);
    }
    return 0;
}

static int writeout(const char* outfn, unsigned char* img, int W, int H, int format) {
    FILE* fout;
    int rtn;
    int outstdout = (!outfn || streq(outfn, "-"));
    if (outstdout) {
        fout = stdout;
    } else {
        fout = fopen(outfn, "wb");
        if (!fout) {
            fprintf(stderr, "Failed to open output file %s: %s\n", outfn, strerror(errno));
            return -1;
        }
    }
    rtn = streamout(fout, img, W, H, format);
    if (rtn)
        return rtn;
    if (!outstdout) {
        if (fclose(fout)) {
            fprintf(stderr, "Failed to close output file %s: %s\n", outfn, strerror(errno));
            return -1;
        }
    }
    return 0;
}

void cairoutils_fake_ppm_init() {
    // 简单实现，不需要初始化
}

int cairoutils_write_ppm(const char* outfn, unsigned char* img, int W, int H) {
    return writeout(outfn, img, W, H, PPM);
}

int cairoutils_write_png(const char* outfn, unsigned char* img, int W, int H) {
    return writeout(outfn, img, W, H, PNG);
}

int cairoutils_write_jpeg(const char* outfn, unsigned char* img, int W, int H) {
    return writeout(outfn, img, W, H, JPEG);
}

int cairoutils_stream_ppm(FILE* fout, unsigned char* img, int W, int H) {
    return streamout(fout, img, W, H, PPM);
}

int cairoutils_stream_png(FILE* fout, unsigned char* img, int W, int H) {
    return streamout(fout, img, W, H, PNG);
}

int cairoutils_stream_jpeg(FILE* fout, unsigned char* img, int W, int H) {
    return streamout(fout, img, W, H, JPEG);
}

void cairoutils_premultiply_alpha_rgba(unsigned char* img, int W, int H) {
    int i;
    for (i=0; i<(H*W); i++) {
        unsigned char r,g,b,a;
        r = img[4*i + 0];
        g = img[4*i + 1];
        b = img[4*i + 2];
        a = img[4*i + 3];
        img[4*i + 0] = (a * r) / 255;
        img[4*i + 1] = (a * g) / 255;
        img[4*i + 2] = (a * b) / 255;
    }
}

void cairoutils_argb32_to_rgba(unsigned char* img, int W, int H) {
    cairoutils_argb32_to_rgba_2(img, img, W, H);
}

void cairoutils_argb32_to_rgba_2(const unsigned char* inimg,
                                 unsigned char* outimg, int W, int H) {
    int i;
    for (i=0; i<(H*W); i++) {
        unsigned char r,g,b,a;
        uint32_t ipix = *((uint32_t*)(inimg + 4*i));
        a = (ipix >> 24) & 0xff;
        r = (ipix >> 16) & 0xff;
        g = (ipix >>  8) & 0xff;
        b = (ipix      ) & 0xff;
        outimg[4*i + 0] = r;
        outimg[4*i + 1] = g;
        outimg[4*i + 2] = b;
        outimg[4*i + 3] = a;
    }
}

void cairoutils_argb32_to_rgba_flip(const unsigned char* inimg,
                                    unsigned char* outimg, int W, int H) {
    int i, j;
    for (i=0; i<H; i++) {
        const unsigned char* inrow = inimg + 4 * (i*W);
        unsigned char* outrow = outimg + 4 * (H-1-i) * W;
        for (j=0; j<(W); j++) {
            unsigned char r,g,b,a;
            uint32_t ipix = *((uint32_t*)(inrow + 4*j));
            a = (ipix >> 24) & 0xff;
            r = (ipix >> 16) & 0xff;
            g = (ipix >>  8) & 0xff;
            b = (ipix      ) & 0xff;
            outrow[4*j + 0] = r;
            outrow[4*j + 1] = g;
            outrow[4*j + 2] = b;
            outrow[4*j + 3] = a;
        }
    }
}

void cairoutils_rgba_to_argb32(unsigned char* img, int W, int H) {
    cairoutils_rgba_to_argb32_2(img, img, W, H);
}

void cairoutils_rgba_to_argb32_2(const unsigned char* inimg,
                                 unsigned char* outimg, int W, int H) {
    int i;
    for (i=0; i<(H*W); i++) {
        unsigned char r,g,b,a;
        uint32_t* ipix;
        r = inimg[4*i + 0];
        g = inimg[4*i + 1];
        b = inimg[4*i + 2];
        a = inimg[4*i + 3];
        ipix = (uint32_t*)(outimg + 4*i);
        *ipix = (a << 24) | (r << 16) | (g << 8) | b;
    }
}

void cairoutils_rgba_to_argb32_flip(const unsigned char* inimg,
                                    unsigned char* outimg, int W, int H) {
    int i, j;
    for (i=0; i<H; i++) {
        const unsigned char* inrow = inimg + 4 * (i*W);
        unsigned char* outrow = outimg + 4 * (H-1-i) * W;
        for (j=0; j<(W); j++) {
            unsigned char r,g,b,a;
            uint32_t* ipix;
            r = inrow[4*j + 0];
            g = inrow[4*j + 1];
            b = inrow[4*j + 2];
            a = inrow[4*j + 3];
            ipix = (uint32_t*)(outrow + 4*j);
            *ipix = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
}


// 跳过PPM文件中的空白字符和注释
static void skip_ppm_whitespace_and_comments(FILE* fin) {
    int c;
    while ((c = fgetc(fin)) != EOF) {
        if (c == '#') {
            // 跳过注释行
            while ((c = fgetc(fin)) != EOF && c != '\n' && c != '\r');
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            // 跳过空白字符
            continue;
        } else {
            // 非空白字符，放回去
            ungetc(c, fin);
            break;
        }
    }
}

// 读取PPM文件中的一个整数
static int read_ppm_int(FILE* fin) {
    int value = 0;
    int c;

    skip_ppm_whitespace_and_comments(fin);

    while ((c = fgetc(fin)) != EOF && c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
    }

    if (c != EOF) {
        ungetc(c, fin);
    }

    return value;
}

unsigned char* cairoutils_read_ppm_stream(FILE* fin, int* pW, int* pH) {
    char magic[3] = {0};
    int W, H, maxval;
    unsigned char* img;
    int x, y;
    int is_binary = 0;

    // 读取PPM魔数
    if (fread(magic, 1, 2, fin) != 2) {
        ERROR("Failed to read PPM magic number");
        return NULL;
    }
    magic[2] = '\0';

    // 检查PPM格式
    if (strcmp(magic, "P3") == 0) {
        is_binary = 0; // ASCII格式
    } else if (strcmp(magic, "P6") == 0) {
        is_binary = 1; // 二进制格式
    } else {
        ERROR("Unsupported PPM format: %s (only P3 and P6 are supported)", magic);
        return NULL;
    }

    // 读取宽度、高度和最大值
    W = read_ppm_int(fin);
    H = read_ppm_int(fin);
    maxval = read_ppm_int(fin);

    if (W <= 0 || H <= 0 || maxval <= 0 || maxval > 65535) {
        ERROR("Invalid PPM dimensions or maxval: %dx%d, maxval=%d", W, H, maxval);
        return NULL;
    }

    // 跳过头部后的第一个空白字符
    skip_ppm_whitespace_and_comments(fin);

    if (pW) *pW = W;
    if (pH) *pH = H;

    // 分配图像内存 (RGBA格式)
    img = malloc(4 * W * H);
    if (!img) {
        ERROR("Failed to allocate image memory for %dx%d image", W, H);
        return NULL;
    }

    // 读取像素数据
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            unsigned char rgb[3];
            int idx = (y * W + x) * 4;

            if (is_binary) {
                // P6格式：二进制数据
                if (maxval < 256) {
                    // 8位数据
                    size_t bytes_read = fread(rgb, 1, 3, fin);
                    if (bytes_read != 3) {
                        if (feof(fin)) {
                            // 文件结尾，用黑色填充剩余像素
                            memset(rgb, 0, 3);
                        } else {
                            ERROR("Failed to read pixel data at (%d,%d), only got %zu bytes", x, y, bytes_read);
                            free(img);
                            return NULL;
                        }
                    }
                } else {
                    // 16位数据，读取并转换为8位
                    unsigned short rgb16[3];
                    size_t shorts_read = fread(rgb16, 2, 3, fin);
                    if (shorts_read != 3) {
                        if (feof(fin)) {
                            // 文件结尾，用黑色填充
                            memset(rgb, 0, 3);
                        } else {
                            ERROR("Failed to read 16-bit pixel data at (%d,%d), only got %zu shorts", x, y, shorts_read);
                            free(img);
                            return NULL;
                        }
                    } else {
                        rgb[0] = (rgb16[0] * 255) / maxval;
                        rgb[1] = (rgb16[1] * 255) / maxval;
                        rgb[2] = (rgb16[2] * 255) / maxval;
                    }
                }
            } else {
                // P3格式：ASCII数据
                int r, g, b;
                r = read_ppm_int(fin);
                g = read_ppm_int(fin);
                b = read_ppm_int(fin);

                if (feof(fin) || r < 0 || g < 0 || b < 0) {
                    // 文件结尾或读取失败，用黑色填充
                    rgb[0] = rgb[1] = rgb[2] = 0;
                } else if (r > maxval || g > maxval || b > maxval) {
                    ERROR("Invalid pixel values at (%d,%d): %d,%d,%d (maxval=%d)", x, y, r, g, b, maxval);
                    free(img);
                    return NULL;
                } else {
                    rgb[0] = (r * 255) / maxval;
                    rgb[1] = (g * 255) / maxval;
                    rgb[2] = (b * 255) / maxval;
                }
            }

            // 转换为RGBA格式
            img[idx + 0] = rgb[0]; // R
            img[idx + 1] = rgb[1]; // G
            img[idx + 2] = rgb[2]; // B
            img[idx + 3] = 255;    // A (不透明)
        }
    }

    return img;
}

unsigned char* cairoutils_read_ppm(const char* infn, int* pW, int* pH) {
    FILE* fin;
    int fromstdin;
    unsigned char* img;

    fromstdin = (infn == NULL) || streq(infn, "-");
    if (!fromstdin) {
        fin = fopen(infn, "rb");
        if (!fin) {
            fprintf(stderr, "Failed to read input image %s: %s\n", infn, strerror(errno));
            return NULL;
        }
    } else {
        fin = stdin;
    }

    img = cairoutils_read_ppm_stream(fin, pW, pH);

    if (!fromstdin) {
        fclose(fin);
    }
    return img;
}

