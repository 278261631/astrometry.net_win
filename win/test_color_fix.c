#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// 简化版的颜色转换函数用于测试
void test_argb32_to_rgba_old(const unsigned char* inimg,
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

void test_argb32_to_rgba_new(const unsigned char* inimg,
                             unsigned char* outimg, int W, int H) {
    int i;
    for (i=0; i<(H*W); i++) {
        unsigned char r,g,b,a;
        
#ifdef _WIN32
        // Windows系统通常是小端序，直接从字节数组读取
        // Cairo ARGB32在小端序系统上的内存布局：[B][G][R][A]
        b = inimg[4*i + 0];
        g = inimg[4*i + 1]; 
        r = inimg[4*i + 2];
        a = inimg[4*i + 3];
#else
        // 其他系统使用原来的位操作方法
        uint32_t ipix = *((uint32_t*)(inimg + 4*i));
        a = (ipix >> 24) & 0xff;
        r = (ipix >> 16) & 0xff;
        g = (ipix >>  8) & 0xff;
        b = (ipix      ) & 0xff;
#endif
        
        outimg[4*i + 0] = r;
        outimg[4*i + 1] = g;
        outimg[4*i + 2] = b;
        outimg[4*i + 3] = a;
    }
}

int main() {
    printf("=== Color Conversion Test ===\n");
    
    // 创建测试数据：模拟Cairo ARGB32格式的数据
    // 在小端序系统上，ARGB32实际在内存中是BGRA顺序
    unsigned char test_data[16] = {
        // 像素1: 蓝色(255,0,0,255) -> 在ARGB32中应该是 0xFF0000FF
        // 在小端序内存中: [FF][00][00][FF] (B G R A)
        0xFF, 0x00, 0x00, 0xFF,
        
        // 像素2: 绿色(0,255,0,255) -> 在ARGB32中应该是 0xFF00FF00  
        // 在小端序内存中: [00][FF][00][FF] (B G R A)
        0x00, 0xFF, 0x00, 0xFF,
        
        // 像素3: 红色(0,0,255,255) -> 在ARGB32中应该是 0xFFFF0000
        // 在小端序内存中: [00][00][FF][FF] (B G R A)
        0x00, 0x00, 0xFF, 0xFF,
        
        // 像素4: 白色(255,255,255,255) -> 在ARGB32中应该是 0xFFFFFFFF
        // 在小端序内存中: [FF][FF][FF][FF] (B G R A)
        0xFF, 0xFF, 0xFF, 0xFF
    };
    
    unsigned char result_old[16];
    unsigned char result_new[16];
    
    printf("Input data (simulating Cairo ARGB32 in little-endian memory):\n");
    for (int i = 0; i < 4; i++) {
        printf("Pixel[%d]: [%02X][%02X][%02X][%02X]\n", 
               i, test_data[i*4], test_data[i*4+1], test_data[i*4+2], test_data[i*4+3]);
    }
    
    // 测试旧方法
    test_argb32_to_rgba_old(test_data, result_old, 2, 2);
    printf("\nOld method results (should be wrong on Windows):\n");
    for (int i = 0; i < 4; i++) {
        printf("Pixel[%d]: R=%d, G=%d, B=%d, A=%d\n", 
               i, result_old[i*4], result_old[i*4+1], result_old[i*4+2], result_old[i*4+3]);
    }
    
    // 测试新方法
    test_argb32_to_rgba_new(test_data, result_new, 2, 2);
    printf("\nNew method results (should be correct):\n");
    for (int i = 0; i < 4; i++) {
        printf("Pixel[%d]: R=%d, G=%d, B=%d, A=%d\n", 
               i, result_new[i*4], result_new[i*4+1], result_new[i*4+2], result_new[i*4+3]);
    }
    
    printf("\nExpected results:\n");
    printf("Pixel[0]: R=0, G=0, B=255, A=255 (Blue)\n");
    printf("Pixel[1]: R=0, G=255, B=0, A=255 (Green)\n");
    printf("Pixel[2]: R=255, G=0, B=0, A=255 (Red)\n");
    printf("Pixel[3]: R=255, G=255, B=255, A=255 (White)\n");
    
    return 0;
}
