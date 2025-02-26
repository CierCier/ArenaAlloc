#include <arena.h>
#include <stddef.h>
#include <stdio.h>

#define debug_print(fmt, ...)                                                  \
  fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)

int main() {
  Arena *arena = ArenaCreate(NULL, 0);

  if (!arena) {
    debug_print("Failed to create arena\n", NULL);
    return 1;
  }

  void *p = ArenaAlloc(arena, 10);
  if (!p) {
    debug_print("Failed to allocate memory\n", NULL);
    return 1;
  }
  ArenaReset(arena);
  ArenaDestroy(arena);
  return 0;
}
