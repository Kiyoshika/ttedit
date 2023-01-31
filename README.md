# ttedit - Terminal Text Edit
This is a dummy project greatly inspired by vi(m) to help me learn the `ncurses` library for building interactive terminal applications.

This project is still extremely young and pretty buggy, it's more so an experiment rather than to serve as full-fledged software (vim already exists, afterall).

## KNOWN ISSUES
* Currently `ttedit` cannot render tabs (`\t`) correctly. So if you open a file with `ttedit` that was written with another editor (like vim), it will be very awkward to use.
* Writing past 255 characters on a line exhibits some odd behaviour - this case is not properly handled
* There is no wrapping if writing past the width of your terminal - technically the line is still written correctly but visually is akward
* `ttedit` does not currently support dynamically resizing your terminal window, so zooming in/out will break the rendering
* Using `w` to jump words will break the cursor position if the line below current line is blank and you use `w` to wrap to the next line (see [this issue](https://github.com/Kiyoshika/ttedit/issues/33)

## Demo Video (as of 28 January 2023)
https://user-images.githubusercontent.com/49159969/215283220-fbedd0f8-dd7c-4204-8eee-7c8d151197a0.mp4

## How to Use
Simply use `ttedit myfile.c` to open the editor. If file does not exist, it will create a new blank one. If file exists, it will read the contents and populate the buffer.

## Building from Source
This is a typical cmake project, you can clone the repo, create a build folder inside and `cmake .. && make` which will create a `ttedit` executable inside `src/` folder within your build folder.

## Commands
There are two modes, VISUAL and EDIT. VISUAL mode allows you to move the cursor around and execute quick commands like deleting lines, jumping blocks, etc. EDIT mode is for writing text to the buffer (screen). By default, `ttedit` starts you in VISUAL mode. Each set of commands will be broken out by their respective modes.

### Edit Mode Commands
* `ESC` - go back to VISUAL mode

### Visual Mode Commands
#### Misc
* `ESC` - clear the command buffer (e.g., if you started typing a number to jump lines but want to cancel, you can press `ESC` to clear the buffer and write a different number)
* `i` (insert) - enter EDIT mode
#### Cursor
* `h` - move cursor left
* `l` - move cursor right
* `k` - move cursor up
* `j` - move cursor down
#### File IO
* `s` (save) - Save the contents of the buffer into the file
* `q` (quit) - Exit the editor without saving (WARNING, IT WILL NOT ASK YOU IF YOU WANT TO SAVE FIRST AFTER MAKING CHANGES)
#### Editing
* `p` (prepend) - move to first non-space character and toggle EDIT mode
* `a` (append) - move to end of buffer and toggle EDIT mode
* `d` (delete) - delete the current line
* `n` (newline) - create new line under cursor and jump to it in EDIT mode
* `shift + h` (highlight) - Start highlighting text. Press `shift + h` again or `ESC` to cancel highlighting
* `shift + c` (copy) - Copy highlighted text
* `shift + p` (paste) - Paste copied text at current cursor position
#### Navigation
* `b` (bottom) - jump to bottom of visual buffer (no scrolling)
* `shift + b` (bottom) - jump to bottom of entire buffer (scrolling if needed)
* `t` (top) - jump to top of visual buffer (no scrolling)
* `shift + t` (top) - jump to top of entire buffer (scrolling if needed)
* `{X}j` (jump) - jump to line `X` (e.g., `23j` will jump to line 23)
* `w` (word) - jump forward one word
* `shift + w` (word) - jump backward one word
* `shift + j` (jump) - jump forward/backward a scope: `{` to `}` or `(` to `)` and vice versa.
