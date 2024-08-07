# clab: Creative Programming Solutions
![Banner](.extra/docs/clab-banner.png)

# About the project
clab project aims to provide creative solutions to interesting programming problems. 

C language has been chosen so that the project can develop solutions from scratch and in the process we will build the necessary data structures and algorithms. 

Also, a solution is very insightful when memory is at center stage. We get closer to the actual hardware and the operating system, getting the maximum performance.

Programming is enjoyable when applied to solve interesting problems (may not be directly used in general day-to-day professional work). The learning and knowledge acquired in the process make a better programmer.

## An Interesting Problem - Orienteering Challenge
Given a maze of m x n size
```
# boundary
. open path
@ checkpoint
S source
G destination
```

Develop a program to find the shortest path from 'S' to 'G', visiting all the '@'s.

Example: 7 x 9 maze
```
#########
#....#.@#
#.@...G.#
#.#.....#
#.##@####
#....S..#
#########
```

Solution:
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

Background:
> I got this problem statement in my college placement, but I was not able to solve it in the given deadline. Later in the holidays I went home and was able to solve it ([solution link](https://github.com/janishar/orienteering-problem)), but with the limited knowledge it was difficult to optimize it. Now, after a decade its exciting to revisit the problem and take it as an opportunity to experiment with many other programming concepts.

### Solution pointers
- If there are N checkpoints the total possible path is N!. Factorial N get very large with increase in N. For example: 
factorial 12 is 479,001,600. 
- To handle this large combination 

## Project Structure
```
.
├── Makefile
├── README.md
├── bin
│   ├── clab [binary]
│   └── clabdev [binary]
├── build
│   ├── dev [directory]
│   └── release [directory]
└── src
    ├── crun
    │   ├── clocktime.c
    │   ├── clocktime.h
    │   ├── croutine.h
    │   ├── pipeline.c
    │   ├── pipeline.h
    │   ├── runpool.c
    │   ├── runpool.h
    │   ├── runtime.c
    │   └── runtime.h
    ├── demo
    │   ├── crundemo.c
    │   ├── crundemo.h
    │   ├── dslibdemo.c
    │   ├── dslibdemo.h
    │   ├── model.c
    │   ├── model.h
    │   ├── pathdemo.c
    │   ├── pathdemo.h
    │   ├── puzzledemo.c
    │   ├── puzzledemo.h
    │   ├── termdemo.c
    │   └── termdemo.h
    ├── dslib
    │   ├── bufferq.c
    │   ├── bufferq.h
    │   ├── datastr.c
    │   ├── datastr.h
    │   ├── destroy.c
    │   ├── destroy.h
    │   ├── dump.c
    │   ├── graph.c
    │   ├── graph.h
    │   ├── hashmap.c
    │   ├── hashmap.h
    │   ├── list.c
    │   ├── list.h
    │   ├── path.c
    │   ├── path.h
    │   ├── queue.c
    │   ├── queue.h
    │   ├── stack.c
    │   ├── stack.h
    │   ├── tree.c
    │   ├── tree.h
    │   ├── util.c
    │   └── util.h
  	├── puzzle
    │   ├── maps.txt
    │   ├── maze.c
    │   ├── maze.h
    │   ├── mazesoln.c
    │   ├── util.c
    │   └── util.h
    ├── term
    │   ├── console.c
    │   └── console.h
    ├── debug
    │   ├── dfunc.c
    │   └── dfunc.h
    └── main.c
    
```

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

## Find this project useful ? :heart:
* Support it by clicking the :star: button on the upper right of this page. :v:

## More on YouTube channel - Unusual Code
Subscribe to the YouTube channel `UnusualCode` for understanding the concepts used in this project:

[![YouTube](https://img.shields.io/badge/YouTube-Subscribe-red?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@unusualcode)

## Contribution
Please feel free to fork it and open a PR (many optimization are possible). You can also add more puzzles in the project.