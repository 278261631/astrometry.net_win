#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astrometry/cairoutils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <png-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    int W, H;
    
    printf("=== PNG Color Analysis Tool ===\n");
    printf("Reading PNG file: %s\n", filename);
    
    // 读取PNG文件
    unsigned char* img = cairoutils_read_png(filename, &W, &H);
    if (!img) {
        printf("Failed to read PNG file\n");
        return 1;
    }
    
    printf("Image size: %dx%d\n", W, H);
    printf("Format: RGBA (4 bytes per pixel)\n\n");
    
    // 分析颜色分布
    int red_pixels = 0, green_pixels = 0, blue_pixels = 0, other_pixels = 0;
    int total_pixels = W * H;
    
    // 统计主要颜色的像素数量
    for (int i = 0; i < total_pixels; i++) {
        int idx = i * 4;
        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];
        unsigned char a = img[idx + 3];
        
        // 忽略透明像素
        if (a < 128) continue;
        
        // 检查是否是纯色或接近纯色
        if (r > 200 && g < 100 && b < 100) {
            red_pixels++;
        } else if (g > 200 && r < 100 && b < 100) {
            green_pixels++;
        } else if (b > 200 && r < 100 && g < 100) {
            blue_pixels++;
        } else {
            other_pixels++;
        }
    }
    
    printf("Color analysis:\n");
    printf("Red-dominant pixels: %d (%.1f%%)\n", red_pixels, (red_pixels * 100.0) / total_pixels);
    printf("Green-dominant pixels: %d (%.1f%%)\n", green_pixels, (green_pixels * 100.0) / total_pixels);
    printf("Blue-dominant pixels: %d (%.1f%%)\n", blue_pixels, (blue_pixels * 100.0) / total_pixels);
    printf("Other pixels: %d (%.1f%%)\n", other_pixels, (other_pixels * 100.0) / total_pixels);
    
    // 显示几个像素的详细信息
    printf("\nSample pixel values (first 10 non-transparent pixels):\n");
    int samples = 0;
    for (int i = 0; i < total_pixels && samples < 10; i++) {
        int idx = i * 4;
        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];
        unsigned char a = img[idx + 3];
        
        if (a >= 128) {  // 只显示不透明的像素
            printf("Pixel[%d]: R=%d, G=%d, B=%d, A=%d\n", i, r, g, b, a);
            samples++;
        }
    }
    
    // 检查是否有明显的颜色通道错乱
    if (blue_pixels > red_pixels * 2 && blue_pixels > green_pixels * 2) {
        printf("\nWARNING: Image appears to have excessive blue pixels.\n");
        printf("This might indicate a color channel swap issue.\n");
    } else if (red_pixels > 0 || green_pixels > 0) {
        printf("\nImage appears to have normal color distribution.\n");
    }
    
    free(img);
    return 0;
}
