#include "demo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "console.h"

/*--------------------CONSOLE DEMO------------------------ */

int console_demo(void) {
  printf("\n--------------CONSOLE DEMO----------------\n");
  const char *frames[8] = {
      // Frame 1: Bomb at rest
      "   _______\n"
      "  /       \\\n"
      " |  BOMB!  |\n"
      "  \\_______/\n"
      "   /     \\\n"
      "  /       \\\n"
      " /         \\\n",

      // Frame 2: Bomb starts to shake
      "   _______\n"
      "  /       \\\n"
      " |  BOMB!  |\n"
      "  \\_______/\n"
      "   /     \\\n"
      "  /  .-.  \\\n"
      " /  (   )  \\\n",

      // Frame 3: Bomb shakes more
      "   _______\n"
      "  /       \\\n"
      " |  BOMB!  |\n"
      "  \\_______/\n"
      "   / \\ / \\\n"
      "  /  /   \\ \n"
      " /  /     \\ \n",

      // Frame 4: Bomb starts to explode
      "   _______\n"
      "  /  * *  \\\n"
      " |  BOMB!  |\n"
      "  \\_______/\n"
      "   /     \\\n"
      "  /  .-.  \\\n"
      " /  (   )  \\\n",

      // Frame 5: Bomb exploding more
      "   ____*____\n"
      "  / * * * * \\\n"
      " |   BOOM!  |\n"
      "  \\_______/\n"
      "   /     \\\n"
      "  /  .-.  \\\n"
      " /  (   )  \\\n",

      // Frame 6: Explosion getting bigger
      "   * * * * *\n"
      "  * * BOOM! *\n"
      " * * * * * * *\n"
      "   \\_____/\n"
      "    /     \\\n"
      "   /       \\\n"
      "  /         \\\n",

      // Frame 7: Big explosion
      "  * * * * * * *\n"
      " * * * * * * * *\n"
      "* * * * * BOOM! * *\n"
      " * * * * * * * *\n"
      "  * * * * * * *\n"
      "   \\_____/\n"
      "    /     \\\n",

      // Frame 8: Explosion dissipates
      "  * * * * *\n"
      " * * * * * \n"
      " * * BOOM! *\n"
      "  * * * * *\n"
      "   \\_____/\n"
      "    /     \\\n"
      "   /       \\\n",
  };

  Console *console = console_create(300);

  while (true) {
    for (int i = 0; i < 8; i++) {
      char *text = (char *)malloc(sizeof(char) * 100);
      snprintf(text, 100, "%s", frames[i]);
      console_render(console, text);
    }
  }

  console_destroy(console);
  printf("--------------CONSOLE DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*--------------------CONSOLE DEMO------------------------ */
