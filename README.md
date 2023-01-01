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
* `i` (insert) - enter EDIT mode
* `h` - move cursor left
* `l` - move cursor right
* `k` - move cursor up
* `j` - move cursor down
* `p` (prepend) - move to first non-space character and toggle EDIT mode
* `a` (append) - move to end of buffer and toggle EDIT mode
