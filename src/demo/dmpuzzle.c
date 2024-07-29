#include "dmpuzzle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dslib/util.h"
#include "../puzzle/maze.h"

int maze_shortest_distance_demo(void) {
  const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";
  // const char maze[] = "..........AB@C.D@..@EFGH*I..J.KL@M@..N..@.....OP@Q$RS..........";

  int rows = 7;
  int cols = 9;

  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char temp[10];
      char data = maze[i * cols + j];
      snprintf(temp, 10, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  maze_find_shortest_distance(arr, rows, cols, "S", "G");

  util_destroy_2d_str_arr(arr, rows, cols);
  return EXIT_SUCCESS;
}
