# TODO

## Bugs
This is a list of known issues.
* For some reason some garbage text like `q^G^A` will randomly appear while writing in EDIT mode
* You can move the cursor beyond the allocated space which can potentially let the user write to bad memory. Need to clamp the cursor to the current maximum-allocated numbers of lines.
* When moving cursor to the middle of some text, it will overwrite the existing text rather than actually inserting it between
* Tab characters (`\t`) are very buggy, convert tabs into spaces to make it easier to work with

## Feature Requests
This is a list of items that I eventually want to add.
* Line numbers (static or relative)
* Line jumping (specific line, beginning, end, etc)
* Scope jumping (parenthesis and brances)
* Word jumping (forward and back)
* Copy/paste buffer
* Undo/redo (up to a maximum memory cap, e.g., 100 commands)
* Add configuration options to allow for things like changing the default tab spaces
* Search and find/replace
* Probably some others to come
