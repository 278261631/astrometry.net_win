#include <stdio.h>
#include <stdlib.h>

#include "os-features.h"
#include "fitsioutils.h"
#include "fitstable.h"
#include "log.h"
#include "errors.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <fits-file>\n", argv[0]);
        return -1;
    }
    
    char* filename = argv[1];
    printf("Testing FITS file: %s\n", filename);
    
    // Try to open the FITS table
    fitstable_t* table = fitstable_open(filename);
    if (!table) {
        ERROR("Failed to open FITS file: %s", filename);
        return -1;
    }
    printf("FITS file opened successfully\n");
    
    // Try to open extension 1
    if (fitstable_open_extension(table, 1)) {
        ERROR("Failed to open extension 1");
        fitstable_close(table);
        return -1;
    }
    printf("Extension 1 opened successfully\n");
    
    // Get number of rows
    int nrows = fitstable_nrows(table);
    printf("Number of rows: %d\n", nrows);
    
    // Try to read FLUX column
    printf("Attempting to read FLUX column...\n");
    tfits_type dtype = fitscolumn_double_type();
    double* flux_vals = fitstable_read_column(table, "FLUX", dtype);
    
    if (flux_vals) {
        printf("Successfully read FLUX column\n");
        printf("First few FLUX values: %.2f, %.2f, %.2f\n", 
               flux_vals[0], flux_vals[1], flux_vals[2]);
        free(flux_vals);
    } else {
        ERROR("Failed to read FLUX column");
    }
    
    fitstable_close(table);
    printf("Test completed\n");
    
    return 0;
}
