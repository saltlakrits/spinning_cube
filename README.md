# Rotating Cube in terminal (notcurses branch)

Seen this online a bunch of times, so wanted to learn how to make it myself.
Will adapt to terminal size, so make the font unreadably small to make it look a little better!

Runs at 60fps with a small font in my terminal, can't get it to go above that though.
Implemented threading for drawing a zbuffer while making a new one, didn't increase performance though.
Chalking this up to limitations of the terminal, but perhaps my program is just really slow!

Learned a lot!

## notcurses

Swapped to notcurses instead of ncurses, just to learn. not using any interesting functionality that isn't in ncurses

## long, too many features, overly verbose

as it is just a toy project where i implemented more and more features mostly to learn, it is a bit of a mess
the functions for building the cube are especially verbose, perhaps because i have little knowledge of math in 3d (i learned as i went to make this)

i might try to rewrite this tersely but i have already spent an inordinate amount of time on this
