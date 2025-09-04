#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitsio.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <fits-file>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    fitsfile *fptr;
    int status = 0;
    int hdunum, hdutype;
    int ncols;
    long nrows;
    char extname[FLEN_VALUE];
    
    // 打开FITS文件
    if (fits_open_file(&fptr, filename, READONLY, &status)) {
        fits_report_error(stderr, status);
        return 1;
    }
    
    printf("File: %s\n", filename);
    
    // 获取HDU数量
    if (fits_get_num_hdus(fptr, &hdunum, &status)) {
        fits_report_error(stderr, status);
        fits_close_file(fptr, &status);
        return 1;
    }
    
    printf("Number of HDUs: %d\n", hdunum);
    
    // 遍历所有HDU
    for (int i = 1; i <= hdunum; i++) {
        printf("\n=== HDU %d ===\n", i);
        
        // 移动到指定HDU
        if (fits_movabs_hdu(fptr, i, &hdutype, &status)) {
            fits_report_error(stderr, status);
            continue;
        }
        
        // 获取扩展名
        extname[0] = '\0';
        fits_read_key(fptr, TSTRING, "EXTNAME", extname, NULL, &status);
        if (status == KEY_NO_EXIST) {
            status = 0; // 重置状态
            strcpy(extname, "(no name)");
        }
        
        printf("  HDU Type: ");
        switch (hdutype) {
            case IMAGE_HDU:
                printf("IMAGE\n");
                break;
            case ASCII_TBL:
                printf("ASCII TABLE\n");
                break;
            case BINARY_TBL:
                printf("BINARY TABLE\n");
                break;
            default:
                printf("UNKNOWN (%d)\n", hdutype);
                break;
        }
        
        printf("  Extension Name: %s\n", extname);
        
        if (hdutype == BINARY_TBL || hdutype == ASCII_TBL) {
            // 获取表格信息
            if (fits_get_num_rows(fptr, &nrows, &status) ||
                fits_get_num_cols(fptr, &ncols, &status)) {
                fits_report_error(stderr, status);
                status = 0;
                continue;
            }
            
            printf("  Number of columns: %d\n", ncols);
            printf("  Number of rows: %d\n", nrows);
            
            printf("  Columns:\n");
            for (int c = 1; c <= ncols; c++) {
                char colname[FLEN_VALUE];
                char coltype[FLEN_VALUE];
                char tunit[FLEN_VALUE];
                char tdisp[FLEN_VALUE];
                long repeat;

                if (fits_get_bcolparms(fptr, c, colname, tunit, coltype,
                                     &repeat, NULL, NULL, NULL, tdisp, &status)) {
                    fits_report_error(stderr, status);
                    status = 0;
                    continue;
                }
                
                printf("    [%d] '%s' (type=%s, repeat=%ld)\n", 
                       c, colname, coltype, repeat);
            }
            
            // 检查是否有我们要找的列
            const char* target_cols[] = {
                "kdtree_data_codes",
                "kdtree_lr_codes", 
                "kdtree_data",
                "kdtree_lr",
                "kdtree_header_codes",
                "kdtree_split_codes",
                "kdtree_range_codes",
                NULL
            };
            
            printf("  Searching for target columns:\n");
            for (int t = 0; target_cols[t]; t++) {
                int colnum;
                int found_status = 0;
                if (fits_get_colnum(fptr, CASEINSEN, (char*)target_cols[t], 
                                  &colnum, &found_status) == 0) {
                    printf("    '%s': FOUND (column %d)\n", target_cols[t], colnum);
                } else {
                    printf("    '%s': NOT FOUND\n", target_cols[t]);
                }
            }
        }
    }
    
    fits_close_file(fptr, &status);
    return 0;
}
