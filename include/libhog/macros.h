#ifndef MACROS_H_
#define MACROS_H_

#define UNUSED(...) (void)(__VA_ARGS__)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum { false, true } bool;

#endif
