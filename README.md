\# Sudoku



A modernisation of a classic Win32 Sudoku program.



The aim of this project is to preserve the original solving engine while carefully modernising the user interface, architecture and codebase without rewriting the application from scratch.



\## Status



⚠️ \*\*This repository is currently under active development.\*\*



The project now builds cleanly as a native x64 Win32 application.



Both \*\*9×9\*\* and \*\*16×16 hexadecimal Sudoku\*\* are supported using the same well-tested solving engine while providing user interfaces tailored to each board size.



The project has moved beyond basic modernisation and is now focused on refinement, usability and continued architectural improvements.



\## Current Features



\- Native x64 build

\- Visual Studio 2017 project

\- Modern Win32 callback signatures

\- Legacy Win16 declarations removed

\- Original backtracking Sudoku solver

\- 9×9 Sudoku support

\- 16×16 hexadecimal Sudoku support

\- Interactive candidate display

\- Mouse-based puzzle editing

\- Undo support

\- Right-click Original/User clue toggling

\- Shared board helper architecture



\## Recent Modernisation



\- Board display logic separated into `SudokuDisplay.cpp`

\- Cleaner separation between application logic and board display logic

\- Simplified cell-selection and value-placement workflow

\- Separate rendering paths for 9×9 and 16×16 boards

\- Painted 4×4 hexadecimal candidate display

\- Clickable hexadecimal value picker

\- Modern helper functions for board operations

\- Simplified dialog architecture with obsolete legacy flow removed

\- Improved readability and maintainability throughout the codebase



\## Board Design



The two board sizes intentionally use different presentation techniques while sharing the same solving engine.



\### 9×9 Sudoku



Each cell retains the original interactive design:



\- One large value button

\- Nine clickable candidate buttons

\- Direct candidate selection

\- Red original clues and black user entries



\### 16×16 Hexadecimal Sudoku



Each cell uses one large control rather than sixteen tiny candidate buttons:



\- Candidates are painted directly in a 4×4 layout

\- Clicking a cell opens the hexadecimal value picker

\- Values are displayed using `0–9` and `A–F`

\- Right-clicking a filled cell toggles its Original/User colour

\- Candidate availability is calculated directly from the Sudoku engine



This avoids the impractical alternative of creating 4096 tiny candidate controls.



\## Editing Behaviour



Filled cells are tracked using three separate pieces of information:



\- `matrix\[]\[]` stores the value

\- `known\[]\[]` identifies cells fixed before solving

\- `orig\[]\[]` controls whether a value is displayed as an original clue or a user entry



Original clues are displayed in red, while user-entered and solver-generated values are displayed in black.



Solver-generated values are intentionally not marked as `known`, because the backtracking solver must remain free to clear and replace them while searching for a solution.



When a completed puzzle is saved and reloaded, all loaded values become fixed entries and may then be toggled between Original and User status.



\## Planned Improvements



\- Additional UI polish

\- Keyboard shortcuts

\- Continued code cleanup

\- Further documentation

\- Optional CMake build support



\## Philosophy



This project demonstrates that mature Win32 software can be modernised incrementally while preserving the reliability of its original algorithms.



Rather than rewriting decades of proven code, each improvement is carefully designed, tested and integrated so that the codebase becomes progressively cleaner, simpler and easier to understand.



The different board presentations are deliberate. Each board size receives the interface best suited to it without compromising the shared solving architecture.



\---



\## Credits



\*\*Paul the LionHeart\*\*  

Author and Developer



\*\*ChatGPT\*\*  

Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter



\---



> \*"Follow the value, not the symptoms. Every dragon leaves a trail."\*

