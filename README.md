# Chip-68
A chip 8 interpreter for 68k macs

!!!THIS IS VERY ALPHA SOFTWARE!!!

      use at your own risk

Chip 68 is a chip-8 and super chip-8 interpreter for
system 7 and maybe system 6 (untested). It runs well enough
in Mini vMac at 1x speed in system 7. The program touches
the file system, it isn't capable of writing, but may
not close files correctly.

This interpreter in chip-8 mode satisfies all of
Timendus' Chip-8 test suite v4.2
https://github.com/Timendus/chip8-test-suite

In super chip mode it fails screen.wait in
the quirks test however, I do not currently 
understand what this is testing for.

To load a program go to file->load PGRM or CMD L
While the menu says you can make programs, you cannot.

to toggle compatiblity mode interpreter-> toggle compat
compatablity mode is important as if it encounters an invalid
instruction the interpreter halts execution. toggle compat or reset.

The keyboard layout is the modern keyboard standard
1-4, 1-z.
