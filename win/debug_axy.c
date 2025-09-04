#include <stdio.h>
#include <stdlib.h>
#include "astrometry/anqfits.h"
#include "astrometry/fitsioutils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <axy-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    anqfits_t* fits = anqfits_open(filename);
    if (!fits) {
        printf("Failed to open FITS file: %s\n", filename);
        return 1;
    }
    
    int nextens = anqfits_n_ext(fits);
    printf("File: %s\n", filename);
    printf("Number of extensions: %d\n", nextens);
    
    // 查找包含X,Y列的扩展
    for (int i = 0; i < nextens; i++) {
        const qfits_table* table = anqfits_get_table_const(fits, i);
        if (!table) continue;
        
        printf("\n=== Extension %d ===\n", i);
        printf("  Number of columns: %d\n", table->nc);
        printf("  Number of rows: %d\n", table->nr);
        
        // 查找X和Y列
        int x_col = -1, y_col = -1;
        for (int c = 0; c < table->nc; c++) {
            const qfits_col* col = table->col + c;
            printf("    [%d] '%s'\n", c, col->tlabel ? col->tlabel : "(null)");
            
            if (col->tlabel) {
                if (strcmp(col->tlabel, "X") == 0) x_col = c;
                if (strcmp(col->tlabel, "Y") == 0) y_col = c;
            }
        }
        
        if (x_col >= 0 && y_col >= 0 && table->nr > 0) {
            printf("\n  Found X,Y columns! Reading coordinates...\n");
            
            // 读取前10个和后10个坐标
            int show_count = 10;
            if (table->nr < show_count * 2) show_count = table->nr / 2;
            
            // 使用qfits读取数据比较复杂，我们简化一下
            printf("  Found X,Y columns at indices %d, %d\n", x_col, y_col);
            printf("  Table has %d rows\n", table->nr);

            // 只显示表格信息，不读取具体数据
            printf("  (Coordinate reading not implemented in this simple debug tool)\n");

        }
    }
    
    anqfits_close(fits);
    return 0;
}
