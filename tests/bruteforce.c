#include <arena.h>
#include <stdio.h>

int main() {
  size_t target = 0x1000; // 4KB
  target *= 1024;         // 4MB

  Arena *arena = ArenaCreate(NULL, 0);
  if (!arena) {
    return 1;
  }

  size_t total = 0;
  while (total < target) {
    void *p = ArenaAlloc(arena, 1024);
    if (!p) {
      printf("Failed to allocate memory at %lu\n", total);
      break;
    }
    total += 1024;
  }

  printf("Total memory allocated: 0x%lx\n", total);

  ArenaDestroy(arena);
  return 0;
}
