
# graph-calc

**graph-calc** - a graphical calculator on the terminal written in C. Run the program and type `help` in the prompt for usage.
Debug messages are printed to `stderr`.

## Features
* Calculating expressions
* Plotting graphs
* Variable replacement
* Store old inputs and outputs

## Future
* Load old inputs to prompt
* New functions (lim, log and more)
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
* [Shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting_yard_algorithm)
* [Reverse polish notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)
* [Escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code)
