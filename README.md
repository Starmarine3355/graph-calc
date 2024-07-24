
# Terminal Graphic Calculator Project

NOTE: executable is compiled on ubuntu machine 

Todo:
	* Help window
	* History window
	* Load command (variable replacement)
	* Replace vars
		* Save variables to file (setting enum)
		* Special vars. If output is calculation, save to 'a'. If plotting iterate x, save to 'x'.
	* Plotting
		* Iterate 'x'
		* Calculate new RPN expression
		* Draw to "square" (one square is one character on the terminal)
		* After achieving this, divide the "square" into smaller "squares" and calculate how "filled" the square is.
		* Different amount of filled means different symbols to be drawn (crude density: # -> ')
	* Nicer comments (Do not use "" for variables, it's annoying)

Future:
	* Multiple graphs (different colours)
	* New option commands
		* Intersect "y1 y2"
	* New math functions (lim)

File Structure:
	* main.c - Main loop, executable
	* history.c - Manages file I/O to store history/settings
	* display.c - Manages terminal I/O to draw graph/different modes/command line and get inputs
	* commands.c - Manages command line input and actions
	* ops.c - Manages math operations

Resources:
	* Coloured output (https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c)
	* ANSI ESC sequences (https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
	* Get stderr output (https://stackoverflow.com/questions/6364180/stderr-what-is-it-what-are-its-common-uses#6364205)
	* Shunting yard algorithm (https://www.youtube.com/watch?v=QzVVjboyb0s)
