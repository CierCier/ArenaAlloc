#include "arena.h"
#include <stdlib.h>

Arena *ArenaCreate(Arena *arena) {
  // If an existing arena is passed, it will be reset.
  if (arena) {
    ArenaReset(arena);
    return arena;
  }

  // If NULL is passed, a new arena will be created.
  Arena *new_arena = (Arena *)malloc(sizeof(Arena));
  if (!new_arena) {
    return NULL;
  }

  new_arena->memory = malloc(ARENA_SIZE);
  if (!new_arena->memory) {
    free(new_arena);
    return NULL;
  }

  new_arena->cursor = new_arena->memory;
  new_arena->next = NULL;

  return new_arena;
}

void ArenaDestroy(Arena *arena) {
  if (!arena) {
    return;
  }

  Arena *current = arena;
  while (current) {
    Arena *next = current->next;
    if (current->memory) {
      free(current->memory);
    }
    free(current);
    current = next;
  }
}

void *ArenaAlloc(Arena *arena, size_t size) {
  if (!arena) {
    return NULL;
  }

  if (size > ARENA_SIZE) {
    // cannot allocate more than ARENA_SIZE for now
    return NULL;
  }

  if ((arena->cursor - arena->memory) + size > ARENA_SIZE) {
    // Extend the arena.
    arena = ArenaExtend(arena);
    if (!arena) {
      return NULL;
    }
    return ArenaAlloc(arena, size);
  }

  void *ptr = arena->cursor;
  arena->cursor += size;
  return ptr;
}

void *ArenaCalloc(Arena *arena, size_t size, size_t count) {
  void *ptr = ArenaAlloc(arena, size * count);
  if (!ptr) {
    return NULL;
  }

  // we memset in 64 bit chunks for optimization
  size_t *p = (size_t *)ptr;
  for (size_t i = 0; i < count; i++) {
    p[i] = 0;
  }

  return ptr;
}

void ArenaReset(Arena *arena) {
  if (!arena) {
    return;
  }

  Arena *current = arena;
  while (current) {
    current->cursor = current->memory;
    current = current->next;
  }
}

Arena *ArenaExtend(Arena *arena) {
  if (!arena) {
    return NULL;
  }

  Arena *new_arena = ArenaCreate(NULL);
  if (!new_arena) {
    return NULL;
  }

  arena->next = new_arena;
  return new_arena;
}

size_t ArenaSize(Arena *arena) {
  if (!arena) {
    return 0;
  }

  size_t size = 0;
  Arena *current = arena;
  while (current) {
    size += ARENA_SIZE;
    current = current->next;
  }
  return size;
}

size_t ArenaUsed(Arena *arena) {
  if (!arena) {
    return 0;
  }

  size_t used = 0;
  Arena *current = arena;
  while (current) {
    used += current->cursor - current->memory;
    current = current->next;
  }
  return used;
}
