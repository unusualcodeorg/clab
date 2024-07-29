
#ifndef CLAB_PUZZLE_MAZE_H
#define CLAB_PUZZLE_MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

int maze_shortest_distance(void);

// skip defines the char which can not be traversed
void maze_find_shortest_distance(char ***arr, unsigned int rows, unsigned int cols, char *start,
                                 char *dest, char *skip);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_H
