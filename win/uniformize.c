#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "os-features.h"
#include "log.h"
#include "errors.h"
#include "ioutils.h"

static const char* OPTIONS = "hn:X:Y:e:";

void print_help(char* progname) {
    printf("Usage: %s [options] <input-xylist> <output-xylist>\n"
           "  -h        : print this help\n"
           "  -n <num>  : number of sources per bin (default: 10)\n"
           "  -X <col>  : name of X column (default: X)\n"
           "  -Y <col>  : name of Y column (default: Y)\n"
           "  -e <ext>  : FITS extension to read (default: 1)\n",
           progname);
}

int main(int argc, char** argv) {
    int c;
    char* xcol = "X";
    char* ycol = "Y";
    int n = 10;
    int ext = 1;
    char* infile = NULL;
    char* outfile = NULL;
    
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'h':
            print_help(argv[0]);
            return 0;
        case 'n':
            n = atoi(optarg);
            break;
        case 'X':
            xcol = optarg;
            break;
        case 'Y':
            ycol = optarg;
            break;
        case 'e':
            ext = atoi(optarg);
            break;
        default:
            print_help(argv[0]);
            return -1;
        }
    }
    
    if (argc - optind != 2) {
        print_help(argv[0]);
        printf("\nGot arguments:");
        for (int i = optind; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n");
        return -1;
    }
    
    infile = argv[optind];
    outfile = argv[optind + 1];
    
    printf("uniformize: Processing %s -> %s\n", infile, outfile);
    printf("uniformize: Using %d sources per bin\n", n);
    
    // For now, just copy the input file to output file (no uniformization)
    // This is a temporary solution to get solve-field working
    printf("uniformize: Copying input to output (no uniformization applied)\n");
    
    // Simple file copy
    if (copy_file(infile, outfile)) {
        ERROR("Failed to copy file from %s to %s", infile, outfile);
        return -1;
    }

    // Ensure file is fully written and closed on Windows
#ifdef _WIN32
    Sleep(100);  // 100ms delay
#endif

    printf("uniformize: File copied successfully\n");
    
    return 0;
}
