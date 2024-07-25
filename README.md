
# graph-calc

**graph-calc** - a graphical calculator on the terminal written in C.
Run the program and type `help` in the prompt for usage.

## Features
* Calculating expressions
* Plotting graphs
* Variables
* Store old inputs and outputs

## Future
* Multiple graphs
* Value and zero of graph
* Graph intersection
* Derivate
* Integrals

## Build
* Use `-lm` to link to `math.h`.
* Example uses `GCC` but any C compiler works.
```sh
cc display.c history.c commands.c ops.c main.c -o graph-calc.exe -lm
```

## Resources
* Shunting yard algorithm [Wiki](https://en.wikipedia.org/wiki/Shunting_yard_algorithm)
* Reverse polish notation [Wiki](https://en.wikipedia.org/wiki/Reverse_Polish_notation)
