# clab: Creative Programming Solutions
![Banner](.extra/docs/clab-banner.png)

# About the project
clab project aims to provide creative solutions to interesting programming problems. 

C language has been chosen so that the project can develop solutions from scratch and in the process we will build the necessary data structures and algorithms. 

Also, a solution is very insightful when memory is at center stage. We get closer to the actual hardware and the operating system, getting the maximum performance.

Programming is enjoyable when applied to solve interesting problems (may not be directly used in general day-to-day professional work). The learning and knowledge acquired in the process make a better programmer.

## An Interesting Problem - Orienteering Challenge
Given a maze of m x n size, 
- '#' boundary
- '.' open path
- '@' check points
- 'S' starting point
- 'G' destination

Develop a program to find the shortest path from 'S' to 'G', visiting all the '@'s.

Example: 7 x 9 matrix
```
#########
#....#.@#
#.@...G.#
#.#.....#
#.##@####
#....S..#
#########
```

Solution
```
Step 1:     Step 2:      Step 3:      Step 4:
#########   #########    #########    #########
#    #  #   #    #  #    #    #.1#    #    #.1#
#       #   # 2..   #    # 2...G #    #     G #
# #     #   # # .   #    # #     #    # #     #
# ##3####   # ##3####    # ## ####    # ## ####
#   .S  #   #       #    #       #    #       #
#########   #########    #########    #########

Maze: shortest travel distance 14
Time taken: 0.000932 sec
```

> I got this problem statement in my college placement, but I was not able to solve it in the given deadline. Later in the holidays I went home and was able to solve it ([solution link](https://github.com/janishar/orienteering-problem)), but with the limited knowledge it was difficult to optimize it. Now, after a decade its exciting to revisit the problem and take it as an opportunity to experiment with many other programming concepts.

## Using Project
The project has been tested on MacOS and Linux (Ubuntu and Kali Linux).

### Build Binaries
```bash
# To build both dev and release binaries
make

# To build only the dev binary
make dev

# To build only the release binary
make release

```

### Run Dev Program
```bash
make runclabdev cmd=stack
# or 
make runclabdev cmd1=debug cmd2=maze_solution_demo
# or
./bin/clabdev stack
# or
./bin/clabdev debug maze_solution_demo
```

### Run Release Program
```bash
make runclab cmd=stack
# or 
make runclab cmd1=debug cmd2=maze_solution_demo
# or
./bin/clab stack
# or
./bin/clab debug maze_solution_demo
```
### To remove all build files and binaries
```bash
make clean
```
