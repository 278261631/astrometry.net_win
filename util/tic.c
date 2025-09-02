/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifndef _WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "tic.h"
#include "errors.h"

#ifdef _WIN32
struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
    long ru_maxrss;
};

static int getrusage(int who, struct rusage *usage) {
    HANDLE process = GetCurrentProcess();
    FILETIME creation, exit, kernel, user;
    ULARGE_INTEGER kernel_time, user_time;

    if (!GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
        return -1;
    }

    kernel_time.LowPart = kernel.dwLowDateTime;
    kernel_time.HighPart = kernel.dwHighDateTime;
    user_time.LowPart = user.dwLowDateTime;
    user_time.HighPart = user.dwHighDateTime;

    /* Convert from 100-nanosecond intervals to microseconds */
    usage->ru_stime.tv_sec = (long)(kernel_time.QuadPart / 10000000ULL);
    usage->ru_stime.tv_usec = (long)((kernel_time.QuadPart % 10000000ULL) / 10);
    usage->ru_utime.tv_sec = (long)(user_time.QuadPart / 10000000ULL);
    usage->ru_utime.tv_usec = (long)((user_time.QuadPart % 10000000ULL) / 10);

    /* Windows doesn't provide RSS info easily, set to 0 */
    usage->ru_maxrss = 0;

    return 0;
}

#define RUSAGE_SELF 0
#endif
#include "log.h"

static time_t starttime;
static double starttime2;
static double startutime, startstime;

double timenow() {
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        ERROR("Failed to get time of day");
        return -1.0;
    }
    return (double)(tv.tv_sec - 3600*24*365*30) + tv.tv_usec * 1e-6;
}

double millis_between(struct timeval* tv1, struct timeval* tv2) {
    return
        (tv2->tv_usec - tv1->tv_usec)*1e-3 +
        (tv2->tv_sec  - tv1->tv_sec )*1e3;
}

void tic() {
    starttime = time(NULL);
    starttime2 = timenow();
    if (get_resource_stats(&startutime, &startstime, NULL)) {
        ERROR("Failed to get_resource_stats()");
        return;
    }
}

int get_resource_stats(double* p_usertime, double* p_systime, long* p_maxrss) {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage)) {
        SYSERROR("Failed to get resource stats (getrusage)");
        return 1;
    }
    if (p_usertime) {
        *p_usertime = usage.ru_utime.tv_sec + 1e-6 * usage.ru_utime.tv_usec;
    }
    if (p_systime) {
        *p_systime = usage.ru_stime.tv_sec + 1e-6 * usage.ru_stime.tv_usec;
    }
    if (p_maxrss) {
        *p_maxrss = usage.ru_maxrss;
    }
    return 0;
}

void toc() {
    double utime, stime;
    long rss;
    //int dtime;
    double dtime2;
    //time_t endtime = time(NULL);
    //dtime = (int)(endtime - starttime);
    dtime2 = timenow() - starttime2;
    if (get_resource_stats(&utime, &stime, &rss)) {
        ERROR("Failed to get_resource_stats()");
        return;
    }
    logmsg("Used %g s user, %g s system (%g s total), %g s wall time since last check\n",
           utime-startutime, stime-startstime, (utime + stime)-(startutime+startstime), dtime2);
}
