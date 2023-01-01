# TODO

## Bugs
This is a list of known issues.
* No major issues known (yet)

## Memory
* Right now, heap memory is not being free'd (`screen_buffer.lines`) since we haven't implemented a write/quit method yet. When that's finished, Ctrl+C will be disabled to prevent the user from exiting application without freeing memory.

## Feature Requests
This is a list of items that I eventually want to add.
* Read/write to files
* Line numbers (static or relative)
* Line jumping
* Scope jumping (parenthesis and brances)
* Word jumping (forward and back)
* Copy/paste buffer
* Undo/redo (up to a maximum memory cap, e.g., 100 commands)
* Add configuration options to allow for things like changing the default tab spaces
* Search and find/replace
* Probably some others to come
