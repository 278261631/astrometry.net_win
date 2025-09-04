#include <stdio.h>
#include <stdlib.h>
#include "astrometry/anqfits.h"
#include "astrometry/qfits_error.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <fits-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    
    // 启用qfits错误输出
    qfits_err_statset(1);
    
    printf("=== Testing anqfits_open with: %s ===\n", filename);
    
    anqfits_t* fits = anqfits_open(filename);
    if (!fits) {
        printf("ERROR: anqfits_open failed\n");
        return 1;
    }
    
    int nextens = anqfits_n_ext(fits);
    printf("SUCCESS: anqfits_open returned %d extensions\n", nextens);
    
    // 显示每个扩展的基本信息
    for (int i = 0; i < nextens; i++) {
        printf("Extension %d:\n", i);
        
        const qfits_header* hdr = anqfits_get_header_const(fits, i);
        if (hdr) {
            printf("  Header: present\n");
        } else {
            printf("  Header: NULL\n");
        }
        
        const qfits_table* table = anqfits_get_table_const(fits, i);
        if (table) {
            printf("  Table: %d cols, %d rows\n", table->nc, table->nr);
        } else {
            printf("  Table: NULL (not a table extension)\n");
        }
    }
    
    anqfits_close(fits);
    return 0;
}
