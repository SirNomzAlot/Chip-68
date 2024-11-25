# Chip-68
A chip 8 interpreter for vintage macintosh

!!!THIS IS VERY ALPHA SOFTWARE!!!<br /> 
       use at your own risk

Chip 68 is a chip-8, super chip-8, super chip-8 v1.1 and
XO-chop interpreter for vintage 68k based macs running 
system 7 and maybe system 6 (untested). It runs well enough
in Mini vMac at x1 speed in system 7. The program touches
the file system, it isn't capable of writing, but may
not close files correctly.

Confirmed working on SE/30 running System 7.

This interpreter in chip-8 mode satisfies all of
Timendus' Chip-8 test suite v4.2<br /> 
https://github.com/Timendus/chip8-test-suite
howevever danm8ku does not work.

In super chip mode it fails screen.wait in
the quirks test however, I do not currently 
understand what this is testing for.

To load a program go to file -> load PGRM or CMD L<br /> 
the menu allows you to choose any file, there is only
basic protections in place to not open something dumb.
While the menu says you can make programs, you cannot.

to change compatabilty mode interpreter -> desired specification<br /> 
compatablity mode is important as if it encounters an invalid
instruction the interpreter halts execution. use the file menu to reset.

The keyboard layout is the modern keyboard standard<br /> 
1-4, 1-z.

to build you need a valid install of retro68, then run<br />
cmake -DCMAKE_TOOLCHAIN_FILE=???/Retro68-build/toolchain/m68k-apple-macos/cmake/retro68.toolchain.cmake
however change the question marks to properly point to your install.
then make.
