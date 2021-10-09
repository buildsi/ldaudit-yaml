// preloadlib.so

#include <stdio.h>
#include <link.h>

__attribute__((constructor))
static void init(void) {
  printf("I'm loaded from LD_PRELOAD ");
}
