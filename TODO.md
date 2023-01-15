# TODO

## Bugs
This is a list of known issues.
* Jumping to a line below 1 (e.g., `0j`) will segfault - no bounds checking done on `screen.start_idx`

## Feature Requests
This is a list of items that I eventually want to add.
* Show the COMMAND BUFFER on the bottom of the screen (maybe towards the bottom right) to make it clearer for user to see what they're typing
* Scope jumping (parenthesis and brances)
* Copy/paste buffer
* Undo/redo (up to a maximum memory cap, e.g., 100 commands)
* Add configuration options to allow for things like changing the default tab spaces
* Search and find/replace
* Probably some others to come
