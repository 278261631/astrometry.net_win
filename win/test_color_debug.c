#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astrometry/cairoutils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ppm-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    int W, H;
    
    printf("=== PPM Color Debug Tool ===\n");
    printf("Reading PPM file: %s\n", filename);
    
    // 读取PPM文件
    unsigned char* img = cairoutils_read_ppm(filename, &W, &H);
    if (!img) {
        printf("Failed to read PPM file\n");
        return 1;
    }
    
    printf("Image size: %dx%d\n", W, H);
    printf("Format: RGBA (4 bytes per pixel)\n\n");
    
    // 显示几个像素的原始RGBA值
    printf("=== Original RGBA values (first 10 pixels) ===\n");
    for (int i = 0; i < 10 && i < W*H; i++) {
        int idx = i * 4;
        printf("Pixel[%d]: R=%d, G=%d, B=%d, A=%d\n", 
               i, img[idx], img[idx+1], img[idx+2], img[idx+3]);
    }
    
    // 创建副本用于转换测试
    unsigned char* img_copy = malloc(4 * W * H);
    memcpy(img_copy, img, 4 * W * H);
    
    // 转换为ARGB32格式
    printf("\n=== Converting to ARGB32 ===\n");
    cairoutils_rgba_to_argb32(img_copy, W, H);
    
    // 显示转换后的值（作为字节数组查看）
    printf("=== ARGB32 bytes (first 10 pixels) ===\n");
    for (int i = 0; i < 10 && i < W*H; i++) {
        int idx = i * 4;
        printf("Pixel[%d]: [0]=%d, [1]=%d, [2]=%d, [3]=%d\n", 
               i, img_copy[idx], img_copy[idx+1], img_copy[idx+2], img_copy[idx+3]);
    }
    
    // 转换回RGBA查看结果
    unsigned char* img_back = malloc(4 * W * H);
    memcpy(img_back, img_copy, 4 * W * H);
    cairoutils_argb32_to_rgba(img_back, W, H);
    
    printf("\n=== Converted back to RGBA ===\n");
    for (int i = 0; i < 10 && i < W*H; i++) {
        int idx = i * 4;
        printf("Pixel[%d]: R=%d, G=%d, B=%d, A=%d\n", 
               i, img_back[idx], img_back[idx+1], img_back[idx+2], img_back[idx+3]);
    }
    
    // 检查是否有颜色通道交换
    printf("\n=== Color Channel Analysis ===\n");
    int r_matches = 0, g_matches = 0, b_matches = 0;
    for (int i = 0; i < 100 && i < W*H; i++) {
        int orig_idx = i * 4;
        int back_idx = i * 4;
        
        if (img[orig_idx] == img_back[back_idx]) r_matches++;
        if (img[orig_idx+1] == img_back[back_idx+1]) g_matches++;
        if (img[orig_idx+2] == img_back[back_idx+2]) b_matches++;
    }
    
    printf("Channel preservation (out of 100 pixels):\n");
    printf("  R channel: %d matches\n", r_matches);
    printf("  G channel: %d matches\n", g_matches);
    printf("  B channel: %d matches\n", b_matches);
    
    if (r_matches < 90 || g_matches < 90 || b_matches < 90) {
        printf("WARNING: Color channels may be swapped!\n");
    }
    
    // 检查一些特定颜色区域
    printf("\n=== Sample Color Analysis ===\n");
    printf("Checking center pixel (%d,%d):\n", W/2, H/2);
    int center_idx = ((H/2) * W + (W/2)) * 4;
    if (center_idx < W*H*4) {
        printf("  Original: R=%d, G=%d, B=%d\n", 
               img[center_idx], img[center_idx+1], img[center_idx+2]);
        printf("  After conversion: R=%d, G=%d, B=%d\n", 
               img_back[center_idx], img_back[center_idx+1], img_back[center_idx+2]);
    }
    
    free(img);
    free(img_copy);
    free(img_back);
    
    printf("\n=== Debug completed ===\n");
    return 0;
}
