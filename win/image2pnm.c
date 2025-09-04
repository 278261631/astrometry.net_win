#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

// Simple image2pnm wrapper for Windows
// This program tries to call the Python version first, 
// then falls back to an-fitstopnm for FITS files

static int file_exists(const char* filename) {
    struct stat st;
    return (stat(filename, &st) == 0);
}

static int is_fits_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    
    char header[6];
    size_t read = fread(header, 1, 6, f);
    fclose(f);
    
    if (read >= 6 && strncmp(header, "SIMPLE", 6) == 0) {
        return 1;
    }
    return 0;
}

static int run_command(const char* cmd) {
#ifdef _WIN32
    return system(cmd);
#else
    return system(cmd);
#endif
}

static int try_python_version(int argc, char* argv[]) {
    char cmd[4096];
    char* python_cmds[] = {"C:\\Python\\Python310\\python.exe", "python", "python3",
                          "C:\\Python39\\python.exe", "C:\\Python310\\python.exe",
                          "C:\\Python311\\python.exe", "C:\\Python312\\python.exe", NULL};

    for (int i = 0; python_cmds[i]; i++) {
        // Test if Python is available
        snprintf(cmd, sizeof(cmd), "%s --version >nul 2>&1", python_cmds[i]);
        if (run_command(cmd) == 0) {
            // Python found, try to run the script with PYTHONPATH set
            // Get current directory
            char current_dir[1024];
#ifdef _WIN32
            if (GetCurrentDirectory(sizeof(current_dir), current_dir) == 0) {
                strcpy(current_dir, ".");
            }
#else
            if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                strcpy(current_dir, ".");
            }
#endif

            // Set PYTHONPATH and run the script
            snprintf(cmd, sizeof(cmd), "set PYTHONPATH=%%PYTHONPATH%%;%s && %s \"../util/image2pnm.py\"",
                    current_dir, python_cmds[i]);

            // Add all arguments
            for (int j = 1; j < argc; j++) {
                strcat(cmd, " \"");
                strcat(cmd, argv[j]);
                strcat(cmd, "\"");
            }

            int result = run_command(cmd);
            // If Python script failed (e.g., missing modules), return -1 to try fallback
            if (result != 0) {
                printf("Python script failed, trying fallback mode...\n");
                return -1;
            }
            return result;
        }
    }
    return -1; // Python not found
}

static int try_netpbm_conversion(const char* input_file, const char* output_file) {
    char cmd[4096];
    char netpbm_path[1024];

    // Get the directory of the current executable
    char exe_path[1024];
#ifdef _WIN32
    if (GetModuleFileName(NULL, exe_path, sizeof(exe_path)) == 0) {
        strcpy(exe_path, ".");
    }
#else
    strcpy(exe_path, ".");
#endif

    // Extract directory from executable path
    char* last_slash = strrchr(exe_path, '\\');
    if (last_slash) {
        *last_slash = '\0';
        snprintf(netpbm_path, sizeof(netpbm_path), "%s", exe_path);
    } else {
        strcpy(netpbm_path, ".");
    }

    // Get file extension to determine conversion method
    const char* ext = strrchr(input_file, '.');
    if (!ext) return -1;

    // Convert to lowercase for comparison
    char ext_lower[10];
    int i;
    for (i = 0; i < 9 && ext[i+1]; i++) {
        ext_lower[i] = tolower(ext[i+1]);
    }
    ext_lower[i] = '\0';

    // Try different netpbm converters based on file extension
    if (strcmp(ext_lower, "jpg") == 0 || strcmp(ext_lower, "jpeg") == 0) {
        snprintf(cmd, sizeof(cmd), "cmd /c \"\"%s\\jpegtopnm.exe\" \"%s\" > \"%s\"\"",
                netpbm_path, input_file, output_file ? output_file : "output.ppm");
    } else if (strcmp(ext_lower, "png") == 0) {
        snprintf(cmd, sizeof(cmd), "cmd /c \"\"%s\\pngtopnm.exe\" \"%s\" > \"%s\"\"",
                netpbm_path, input_file, output_file ? output_file : "output.ppm");
    } else if (strcmp(ext_lower, "gif") == 0) {
        snprintf(cmd, sizeof(cmd), "cmd /c \"\"%s\\giftopnm.exe\" \"%s\" > \"%s\"\"",
                netpbm_path, input_file, output_file ? output_file : "output.ppm");
    } else if (strcmp(ext_lower, "tiff") == 0 || strcmp(ext_lower, "tif") == 0) {
        snprintf(cmd, sizeof(cmd), "cmd /c \"\"%s\\tifftopnm.exe\" \"%s\" > \"%s\"\"",
                netpbm_path, input_file, output_file ? output_file : "output.ppm");
    } else if (strcmp(ext_lower, "bmp") == 0) {
        snprintf(cmd, sizeof(cmd), "cmd /c \"\"%s\\bmptopnm.exe\" \"%s\" > \"%s\"\"",
                netpbm_path, input_file, output_file ? output_file : "output.ppm");
    } else {
        return -1; // Unsupported format
    }

    printf("Running netpbm command: %s\n", cmd);
    int result = run_command(cmd);
    printf("Netpbm command result: %d\n", result);
    return result;
}

static int fallback_fits_conversion(int argc, char* argv[]) {
    char cmd[4096];
    char* input_file = NULL;
    char* output_file = NULL;
    char* uncompressed_outfile = NULL;
    int extension = 0;
    int ppm_mode = 0;

    // Parse basic arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--infile") == 0) {
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                i++;
            }
        } else if (strcmp(argv[i], "--outfile") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i++;
            }
        } else if (strcmp(argv[i], "--uncompressed-outfile") == 0) {
            if (i + 1 < argc) {
                uncompressed_outfile = argv[i + 1];
                i++;
            }
        } else if (strcmp(argv[i], "--extension") == 0) {
            if (i + 1 < argc) {
                extension = atoi(argv[i + 1]);
                i++;
            }
        } else if (strcmp(argv[i], "--ppm") == 0) {
            ppm_mode = 1;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        return 1;
    }

    // Check if it's a FITS file
    if (is_fits_file(input_file)) {
        // Use standard fitstopnm for FITS files
        snprintf(cmd, sizeof(cmd), "fitstopnm \"%s\"", input_file);

        if (output_file) {
            strcat(cmd, " > \"");
            strcat(cmd, output_file);
            strcat(cmd, "\"");
        }

        printf("Running FITS conversion: %s\n", cmd);
        int result = run_command(cmd);
        printf("FITS conversion result: %d\n", result);
        return result;
    } else {
        // Try netpbm tools for other image formats
        printf("Trying netpbm tools for image conversion...\n");

        char* target_file = output_file;
        if (!target_file && uncompressed_outfile) {
            target_file = uncompressed_outfile;
        }

        if (try_netpbm_conversion(input_file, target_file) == 0) {
            printf("Netpbm conversion successful\n");
            return 0;
        }

        fprintf(stderr, "Error: Image conversion failed\n");
        fprintf(stderr, "Supported methods:\n");
        fprintf(stderr, "1. Use FITS format images\n");
        fprintf(stderr, "2. Install netpbm tools in C:\\GnuWin32\\bin\n");
        fprintf(stderr, "3. Supported formats: JPEG, PNG, GIF, TIFF, BMP\n");
        return 1;
    }
}

int main(int argc, char* argv[]) {
    // First try to use the Python version
    int result = try_python_version(argc, argv);
    
    if (result != -1) {
        // Python version was attempted (success or failure)
        return result;
    }
    
    // Python not available, try fallback for FITS files
    printf("Python not found, using fallback mode for FITS files only...\n");
    return fallback_fits_conversion(argc, argv);
}
