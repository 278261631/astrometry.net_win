#include <stdio.h>
#include <stdlib.h>
#include "astrometry/anqfits.h"
#include "astrometry/fitsioutils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <fits-file>\n", argv[0]);
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
    
    for (int i = 0; i < nextens; i++) {
        printf("\n=== Extension %d ===\n", i);
        
        const qfits_table* table = anqfits_get_table_const(fits, i);
        if (!table) {
            printf("  Not a table extension\n");
            continue;
        }
        
        printf("  Number of columns: %d\n", table->nc);
        printf("  Number of rows: %d\n", table->nr);
        printf("  Table width: %d bytes\n", table->tab_w);
        
        printf("  Columns:\n");
        for (int c = 0; c < table->nc; c++) {
            const qfits_col* col = table->col + c;
            printf("    [%d] '%s' (type=%d, size=%d, nb=%d)\n", 
                   c, col->tlabel ? col->tlabel : "(null)", 
                   col->atom_type, col->atom_size, col->atom_nb);
        }
        
        // 检查是否有我们要找的列
        const char* target_cols[] = {
            "kdtree_data_codes",
            "kdtree_lr_codes", 
            "kdtree_data",
            "kdtree_lr",
            NULL
        };
        
        printf("  Searching for target columns:\n");
        for (int t = 0; target_cols[t]; t++) {
            int found = fits_find_column(table, target_cols[t]);
            printf("    '%s': %s\n", target_cols[t], 
                   found >= 0 ? "FOUND" : "NOT FOUND");
        }
    }
    
    anqfits_close(fits);
    return 0;
}
