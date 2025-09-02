#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the necessary headers
#include "fileutils.h"
#include "ioutils.h"

int main(int argc, char* argv[]) {
    char* me;
    char* engine_path;

    printf("Testing find_executable function...\n");
    printf("argv[0] = %s\n", argv[0]);

    // Test file_executable directly
    printf("\nTesting file_executable directly:\n");
    printf("file_executable(\"astrometry-engine.exe\"): %s\n",
           file_executable("astrometry-engine.exe") ? "TRUE" : "FALSE");
    printf("file_executable(\"./astrometry-engine.exe\"): %s\n",
           file_executable("./astrometry-engine.exe") ? "TRUE" : "FALSE");
    printf("file_executable(\"test-find-executable.exe\"): %s\n",
           file_executable("test-find-executable.exe") ? "TRUE" : "FALSE");

    // Get the path to this executable
    me = find_executable(argv[0], NULL);
    printf("\nMy executable path: %s\n", me ? me : "NULL");

    // Try to find astrometry-engine
    engine_path = find_executable("astrometry-engine", me);
    printf("astrometry-engine path: %s\n", engine_path ? engine_path : "NULL");

    // Try with .exe extension
    if (!engine_path) {
        engine_path = find_executable("astrometry-engine.exe", me);
        printf("astrometry-engine.exe path: %s\n", engine_path ? engine_path : "NULL");
    }

    // Try with current directory
    if (!engine_path) {
        engine_path = find_executable("astrometry-engine.exe", "./test-find-executable.exe");
        printf("astrometry-engine.exe with sibling path: %s\n", engine_path ? engine_path : "NULL");
    }

    // Check current directory
    printf("\nChecking current directory files:\n");
    system("dir *.exe");

    // Check PATH environment
    char* path_env = getenv("PATH");
    printf("\nPATH environment variable (first 200 chars):\n%.200s...\n", path_env ? path_env : "NULL");

    if (me) free(me);
    if (engine_path) free(engine_path);

    return 0;
}
