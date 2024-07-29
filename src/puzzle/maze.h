
#ifndef CLAB_PUZZLE_MAZE_H
#define CLAB_PUZZLE_MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

int maze_shortest_distance(void);
void maze_find_shortest_distance(char ***arr, int rows, int cols, char *start, char *dest);

#ifdef __cplusplus
}
#endif

#endif  // CLAB_PUZZLE_MAZE_H
