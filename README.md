# Study C language

To remove all build files and binaries
```bash
make clean
```

To build both dev and release binaries, run:
```bash
make dev
```

To build only the dev binary, run:
```bash
make dev
```

To build only the release binary, run:
```bash
make release
```

To run the dev binary, run:
```bash
make runclabdev cmd=stack
# or 
make runclabdev cmd1=debug cmd2=maze_shortest_distance
# or
./bin/clabdev stack
# or
./bin/clabdev stack maze_shortest_distance
```

To run the release binary, run:
```bash
make runclab cmd=stack
# or 
make runclab cmd1=debug cmd2=maze_shortest_distance
# or
./bin/clab stack
# or
./bin/clab stack maze_shortest_distance
```

