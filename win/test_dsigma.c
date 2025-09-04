#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// 包含必要的头文件
#include "astrometry/dimage.h"
#include "astrometry/log.h"

int main(int argc, char** argv) {
    printf("=== Testing dsigma function ===\n");
    
    // 创建一个简单的测试图像
    int nx = 100, ny = 100;
    float* image = malloc(nx * ny * sizeof(float));
    
    // 填充测试数据
    printf("Creating test image %dx%d...\n", nx, ny);
    srand(time(NULL));
    for (int i = 0; i < nx * ny; i++) {
        // 添加一些随机噪声
        image[i] = (float)(rand() % 1000) / 1000.0f + 100.0f;
    }
    
    printf("Test image created with values ranging from ~100 to ~101\n");
    
    // 测试dsigma函数
    float sigma = 0.0;
    printf("Calling dsigma...\n");
    fflush(stdout);
    
    int result = dsigma(image, nx, ny, 5, 0, &sigma);
    
    printf("dsigma returned: %d\n", result);
    printf("Calculated sigma: %f\n", sigma);
    
    if (result == 0) {
        printf("ERROR: dsigma failed!\n");
    } else {
        printf("SUCCESS: dsigma completed normally\n");
    }
    
    // 测试一些边界情况
    printf("\n=== Testing edge cases ===\n");
    
    // 测试全零图像
    memset(image, 0, nx * ny * sizeof(float));
    printf("Testing all-zero image...\n");
    result = dsigma(image, nx, ny, 5, 0, &sigma);
    printf("All-zero result: %d, sigma: %f\n", result, sigma);
    
    // 测试常数图像
    for (int i = 0; i < nx * ny; i++) {
        image[i] = 42.0f;
    }
    printf("Testing constant image (value=42)...\n");
    result = dsigma(image, nx, ny, 5, 0, &sigma);
    printf("Constant result: %d, sigma: %f\n", result, sigma);
    
    // 测试包含NaN的图像
    image[0] = NAN;
    image[1] = INFINITY;
    printf("Testing image with NaN and Inf...\n");
    result = dsigma(image, nx, ny, 5, 0, &sigma);
    printf("NaN/Inf result: %d, sigma: %f\n", result, sigma);
    
    // 测试非常小的图像
    printf("Testing tiny image (1x1)...\n");
    float tiny_image = 5.0f;
    result = dsigma(&tiny_image, 1, 1, 5, 0, &sigma);
    printf("Tiny image result: %d, sigma: %f\n", result, sigma);
    
    free(image);
    
    printf("\n=== dsigma test completed ===\n");
    return 0;
}
