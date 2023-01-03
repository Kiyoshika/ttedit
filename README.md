# ttedit - Terminal Text Edit
This is a dummy project greatly inspired by vi(m) to help me learn the `ncurses` library for building interactive terminal applications.

This project is still extremely young and pretty buggy, it's more so an experiment rather than to serve as full-fledged software (vim already exists, afterall).

## Building from Source
This is a typical cmake project, you can clone the repo, create a build folder inside and `cmake .. && make` which will create a `ttedit` executable inside `src/` folder within your build folder.

## Commands
There are two modes, VISUAL and EDIT. VISUAL mode allows you to move the cursor around and execute quick commands like deleting lines, jumping blocks, etc. EDIT mode is for writing text to the buffer (screen). By default, `ttedit` starts you in VISUAL mode. Each set of commands will be broken out by their respective modes.

### Edit Mode Commands
* `ESC` - go back to VISUAL mode

### Visual Mode Commands
* `ESC` - clear the command buffer (e.g., if you started typing a number to jump lines but want to cancel, you can press `ESC` to clear the buffer and write a different number)
* `i` (insert) - enter EDIT mode
* `h` - move cursor left
* `l` - move cursor right
* `k` - move cursor up
* `j` - move cursor down
* `p` (prepend) - move to first non-space character and toggle EDIT mode
* `a` (append) - move to end of buffer and toggle EDIT mode
* `d` (delete) - delete the current line
* `n` (newline) - create new line under cursor and jump to it in EDIT mode
* `b` (bottom) - jump to bottom of visual buffer (no scrolling)
* `shift + b` (bottom) - jump to bottom of entire buffer (scrolling if needed)
* `t` (top) - jump to top of visual buffer (no scrolling)
* `shift + t` (top) - jump to top of entire buffer (scrolling if needed)
* `{X}j` (jump) - jump to line `X` (e.g., `23j` will jump to line 23)
