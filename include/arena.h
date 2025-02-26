// License: MIT
// Author: Aabish Malik (aabishmalik3337@gmail.com)

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

// Use 64MB as the minimum size for the arena.
// Not recommened to decrease this value. It will increase the number of
// syscalls
#define ARENA_MIN_SIZE 0x10000

/* The Arena Structure simply holds the pointer to the memory, the current
 * cursor position and a pointer to the next arena. We simply move a pointer
 * forward until we reach the end of the Arena.
 *
 * Unline malloc, we dont need to keep track of free blocks.
 * By the Current Nature of the Implementation, we cannot create allocations
 * larger than ARENA_SIZE.
 */
typedef struct Arena {
  void *memory;
  void *cursor;
  size_t size;
  struct Arena *next;
} Arena;

/*
 * Create a new arena or initialize an existing one.
 * If an existing arena is passed, it will be reset.
 * If NULL is passed, a new arena will be created.
 * Size of arena is max(ARENA_MIN_SIZE, size).
 */
Arena *ArenaCreate(Arena *arena, size_t size);

/*
 * Destroy an arena and free the memory.
 * Careful, this will free all the memory allocated from the arena, Including
 * any other arenas linked to it.
 */
void ArenaDestroy(Arena *arena);

/*
 * Allocate memory from the arena.
 * The memory is not initialized.
 * Same signature as malloc.
 * If the memory is not available, we try to extend the arena and call
 * ArenaAlloc recursively.
 */
void *ArenaAlloc(Arena *arena, size_t size);

/*
 * Allocate memory from the arena and initialize it to zero.
 * Same signature as calloc. The memory is initialized to zero.
 * If the memory is not available, we try to extend the arena.
 */
void *ArenaCalloc(Arena *arena, size_t size, size_t count);

/*
 * Reset the arena.
 * This will free all memory allocated from the arena and sub-arenas.
 * Very Hacky, just set the cursor to the start of the memory. :P
 */
void ArenaReset(Arena *arena);

/*
 * Extend the arena.
 * This will allocate a new arena and link it to the existing one.
 * Useful when you need more memory.
 * Returns the new arena or NULL if memory allocation fails.
 */
Arena *ArenaExtend(Arena *arena, size_t size);

/*
 * Get the total size of the arena.
 * Including all the sub-arenas.
 */
size_t ArenaSize(Arena *arena);

/*
 * Get the total used size of the arena.
 * Including all the sub-arenas.
 */
size_t ArenaUsed(Arena *arena);

#ifdef __cplusplus
}
#endif
