#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "astrometry/anqfits.h"
#include "astrometry/qfits_std.h"

// 启用调试输出
#define DEBUG_QFITS 1

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <fits-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    FILE* fin = NULL;
    struct stat sta;
    char buf[FITS_BLOCK_SIZE];
    size_t n_blocks = 0;
    int found_it = 0;
    int firsttime = 1;
    
    printf("=== Detailed QFITS Debug for: %s ===\n", filename);
    
    // 检查文件状态
    if (stat(filename, &sta) != 0) {
        printf("ERROR: Cannot stat file %s: %s\n", filename, strerror(errno));
        return 1;
    }
    
    printf("File size: %lld bytes (%lld FITS blocks)\n", 
           (long long)sta.st_size, 
           (long long)(sta.st_size / FITS_BLOCK_SIZE));
    
    // 打开文件
    fin = fopen(filename, "rb");  // 使用二进制模式
    if (!fin) {
        printf("ERROR: Cannot open file %s: %s\n", filename, strerror(errno));
        return 1;
    }
    
    // 读取第一个块
    printf("\n=== Reading Primary HDU ===\n");
    if (fread(buf, 1, FITS_BLOCK_SIZE, fin) != FITS_BLOCK_SIZE) {
        printf("ERROR: Failed to read first block: %s\n", strerror(errno));
        fclose(fin);
        return 1;
    }
    
    // 检查FITS魔数
    if (strncmp(buf, "SIMPLE  =", 9) != 0) {
        printf("ERROR: Not a FITS file (no SIMPLE header)\n");
        printf("First 20 bytes: ");
        for (int i = 0; i < 20; i++) {
            printf("%02x ", (unsigned char)buf[i]);
        }
        printf("\n");
        fclose(fin);
        return 1;
    }
    
    printf("✓ FITS magic number found\n");
    n_blocks = 1;
    
    // 解析主头部
    found_it = 0;
    firsttime = 1;
    int header_blocks = 0;
    
    while (!found_it) {
        if (!firsttime) {
            if (fread(buf, 1, FITS_BLOCK_SIZE, fin) != FITS_BLOCK_SIZE) {
                printf("ERROR: Failed to read header block %zu\n", n_blocks + 1);
                fclose(fin);
                return 1;
            }
            n_blocks++;
        }
        firsttime = 0;
        header_blocks++;
        
        // 查找END关键字
        for (int i = 0; i < FITS_BLOCK_SIZE; i += 80) {
            if (strncmp(buf + i, "END     ", 8) == 0) {
                found_it = 1;
                break;
            }
        }
        
        printf("  Header block %d: %s END\n", header_blocks, found_it ? "found" : "no");
    }
    
    printf("Primary HDU header: %d blocks\n", header_blocks);
    
    // 检查EXTEND关键字
    fseeko(fin, 0, SEEK_SET);
    int has_extensions = 0;
    for (int block = 0; block < header_blocks; block++) {
        if (fread(buf, 1, FITS_BLOCK_SIZE, fin) != FITS_BLOCK_SIZE) {
            printf("ERROR: Failed to re-read header block %d\n", block);
            break;
        }
        
        for (int i = 0; i < FITS_BLOCK_SIZE; i += 80) {
            if (strncmp(buf + i, "EXTEND  =", 9) == 0) {
                if (buf[i + 29] == 'T') {
                    has_extensions = 1;
                    printf("✓ EXTEND = T found\n");
                }
                break;
            }
        }
    }
    
    if (!has_extensions) {
        printf("No extensions declared\n");
        fclose(fin);
        return 0;
    }
    
    // 现在查找扩展
    printf("\n=== Searching for Extensions ===\n");
    
    // 定位到主数据区域后
    fseeko(fin, header_blocks * FITS_BLOCK_SIZE, SEEK_SET);
    
    int ext_num = 1;
    int end_of_file = 0;
    
    while (!end_of_file && ext_num <= 20) {  // 限制最多20个扩展
        printf("\n--- Looking for Extension %d ---\n", ext_num);
        
        // 查找XTENSION头
        found_it = 0;
        size_t search_blocks = 0;
        
        while (!found_it && !end_of_file && search_blocks < 100) {  // 限制搜索范围
            if (fread(buf, 1, FITS_BLOCK_SIZE, fin) != FITS_BLOCK_SIZE) {
                printf("  End of file reached after %zu search blocks\n", search_blocks);
                end_of_file = 1;
                break;
            }
            
            search_blocks++;
            n_blocks++;
            
            if (strncmp(buf, "XTENSION=", 9) == 0) {
                printf("  ✓ Found XTENSION at block %zu (search block %zu)\n", n_blocks, search_blocks);
                found_it = 1;
            } else {
                printf("  Block %zu: No XTENSION (starts with: %.20s)\n", n_blocks, buf);
            }
        }
        
        if (!found_it) {
            printf("  Failed to find XTENSION after %zu blocks\n", search_blocks);
            break;
        }
        
        // 解析扩展头部
        found_it = 0;
        firsttime = 1;
        int ext_header_blocks = 0;
        
        while (!found_it && !end_of_file) {
            if (!firsttime) {
                if (fread(buf, 1, FITS_BLOCK_SIZE, fin) != FITS_BLOCK_SIZE) {
                    printf("  ERROR: Failed to read extension header block\n");
                    end_of_file = 1;
                    break;
                }
                n_blocks++;
            }
            firsttime = 0;
            ext_header_blocks++;
            
            // 查找END关键字
            for (int i = 0; i < FITS_BLOCK_SIZE; i += 80) {
                if (strncmp(buf + i, "END     ", 8) == 0) {
                    found_it = 1;
                    break;
                }
            }
            
            printf("    Extension %d header block %d: %s END\n", 
                   ext_num, ext_header_blocks, found_it ? "found" : "no");
        }
        
        if (found_it) {
            printf("  Extension %d header: %d blocks\n", ext_num, ext_header_blocks);
            ext_num++;
        }
        
        // 简单跳过数据区域（假设没有数据或很小的数据）
        // 在实际实现中，这里需要解析NAXIS等关键字来确定数据大小
    }
    
    printf("\nFound %d extensions total\n", ext_num - 1);
    
    fclose(fin);
    return 0;
}
