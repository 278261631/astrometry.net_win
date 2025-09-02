/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */
#include <string.h>
#include <libgen.h>

#include "fileutils.h"
#include "ioutils.h"
#include "os-features.h"

char* resolve_path(const char* filename, const char* basedir) {
    // we don't use canonicalize_file_name() because it requires the paths
    // to actually exist, while this function should work for output files
    // that don't already exist.
    char* path;
    char* rtn;
    // absolute path?
    if (filename[0] == '/')
        //return strdup(filename);
        return an_canonicalize_file_name(filename);
    asprintf_safe(&path, "%s/%s", basedir, filename);
    //return path;
    rtn = an_canonicalize_file_name(path);
    free(path);
    return rtn;
}

char* find_executable(const char* progname, const char* sibling) {
    char* sib;
    char* sibdir;
    char* path;
    char* pathenv;
    char* progname_with_ext = NULL;

#ifdef _WIN32
    // On Windows, add .exe extension if not present
    if (!strstr(progname, ".exe")) {
        asprintf_safe(&progname_with_ext, "%s.exe", progname);
        progname = progname_with_ext;
    }

    // If it's an absolute path, just return it.
    if (progname[0] == '/' || (progname[1] == ':' && progname[0] != '\0'))
        return strdup(progname);

    // If it's a relative path, resolve it.
    if (strchr(progname, '/') || strchr(progname, '\\')) {
        path = an_canonicalize_file_name(progname);
        if (path && file_executable(path)) {
            if (progname_with_ext) free(progname_with_ext);
            return path;
        }
        free(path);
    }

    // On Windows, if no path separators, check current directory first
    if (file_executable(progname)) {
        char* result = strdup(progname);
        if (progname_with_ext) free(progname_with_ext);
        return result;
    }
#else
    // If it's an absolute path, just return it.
    if (progname[0] == '/')
        return strdup(progname);

    // If it's a relative path, resolve it.
    if (strchr(progname, '/')) {
        path = an_canonicalize_file_name(progname);
        if (path && file_executable(path))
            return path;
        free(path);
    }
#endif

    // If "sibling" contains a path separator, then check relative to it.
#ifdef _WIN32
    if (sibling && (strchr(sibling, '/') || strchr(sibling, '\\'))) {
#else
    if (sibling && strchr(sibling, '/')) {
#endif
        // dirname() overwrites its arguments, so make a copy...
        sib = strdup(sibling);
        sibdir = strdup(dirname(sib));
        free(sib);

#ifdef _WIN32
        asprintf_safe(&path, "%s\\%s", sibdir, progname);
#else
        asprintf_safe(&path, "%s/%s", sibdir, progname);
#endif
        free(sibdir);

        if (file_executable(path)) {
            if (progname_with_ext) free(progname_with_ext);
            return path;
        }

        free(path);
    }

    // Search PATH.
    pathenv = getenv("PATH");
    if (pathenv) {
        while (1) {
            char* separator;
            int len;
            if (!strlen(pathenv))
                break;
#ifdef _WIN32
            separator = strchr(pathenv, ';');
#else
            separator = strchr(pathenv, ':');
#endif
            if (separator)
                len = separator - pathenv;
            else
                len = strlen(pathenv);
#ifdef _WIN32
            if (pathenv[len - 1] == '\\')
#else
            if (pathenv[len - 1] == '/')
#endif
                len--;
#ifdef _WIN32
            asprintf_safe(&path, "%.*s\\%s", len, pathenv, progname);
#else
            asprintf_safe(&path, "%.*s/%s", len, pathenv, progname);
#endif
            if (file_executable(path)) {
                if (progname_with_ext) free(progname_with_ext);
                return path;
            }
            free(path);
            if (separator)
                pathenv = separator + 1;
            else
                break;
        }
    }

    // Not found.
    if (progname_with_ext) free(progname_with_ext);
    return NULL;
}

char* an_canonicalize_file_name(const char* fn) {
    sl* dirs;
    int i;
    char* result;
    // Ugh, special cases.
    if (streq(fn, ".") || streq(fn, "/"))
        return strdup(fn);

    dirs = sl_split(NULL, fn, "/");
    for (i=0; i<sl_size(dirs); i++) {
        if (streq(sl_get(dirs, i), "")) {
            // don't remove '/' from beginning of path!
            if (i) {
                sl_remove(dirs, i);
                i--;
            }
        } else if (streq(sl_get(dirs, i), ".")) {
            sl_remove(dirs, i);
            i--;
        } else if (streq(sl_get(dirs, i), "..")) {
            // don't remove ".." at start of path.
            if (!i)
                continue;
            // don't remove chains of '../../../' at the start.
            if (streq(sl_get(dirs, i-1), ".."))
                continue;
            // but do collapse '/../' to '/' at the start.
            if (streq(sl_get(dirs, i-1), "")) {
                sl_remove(dirs, i);
                i--;
            } else {
                sl_remove(dirs, i-1);
                sl_remove(dirs, i-1);
                i -= 2;
            }
        }
    }
    result = sl_join(dirs, "/");
    sl_free2(dirs);
    return result;
}

