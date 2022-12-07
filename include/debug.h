#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

#define INFO(FILE, msg, ...) do {\
    fprintf(FILE, "file: %s, line: %d:\n", __FILE__, __LINE__);\
    fprintf(FILE, msg, ##__VA_ARGS__);\
    fputs("\n", FILE);\
} while (0)

#define LOGINFO(msg, ...) INFO(stdout, msg, ##__VA_ARGS__)
#ifdef DEBUG
#define DBGINFO(msg, ...) INFO(stdout, msg, ##__VA_ARGS__)
#else
#define DBGINFO(msg, ...)
#endif
#define ERRINFO(msg, ...) INFO(stderr, msg, ##__VA_ARGS__)

#endif