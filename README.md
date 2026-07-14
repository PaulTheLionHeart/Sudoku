\# Sudoku



A modernisation of a classic Win32 Sudoku program.



The aim of this project is to preserve the original solving engine while

carefully modernising the user interface, architecture and codebase without

rewriting the application from scratch.



\## Status



⚠️ \*\*This repository is currently under active development.\*\*



The project now builds cleanly as a native x64 Win32 application.



Both \*\*9×9\*\* and \*\*16×16 hexadecimal Sudoku\*\* are supported using the same

well-tested solving engine while providing user interfaces tailored to each

board size.



The project has moved beyond basic modernisation and is now focused on

refinement, usability and continued architectural improvements.



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

\- Original/User clue editing

\- Shared board helper architecture



\## Recent Modernisation



\- Board rendering separated into `SudokuDisplay.cpp`

\- Cleaner separation between application logic and display logic

\- Simplified editing workflow

\- Separate rendering paths for 9×9 and 16×16 boards

\- Owner-painted hexadecimal candidate display

\- Modern helper functions for board operations

\- Simplified dialog architecture with reduced legacy code

\- Improved readability and maintainability throughout the codebase



\## Planned Improvements



\- Additional UI polish

\- Keyboard shortcuts

\- Continued code cleanup

\- Further documentation

\- Optional CMake build support



\## Philosophy



This project demonstrates that mature Win32 software can be modernised

incrementally while preserving the reliability of the original algorithms.



Rather than rewriting decades of proven code, each improvement is carefully

designed, tested and integrated so that the codebase becomes progressively

cleaner, simpler and easier to understand.



The 9×9 and 16×16 boards intentionally use different presentation techniques

while sharing the same solving engine. This allows each board size to provide

the most natural user experience without compromising the underlying

architecture.



\---



\## Credits



\*\*Paul the LionHeart\*\*  

Author and Developer



\*\*ChatGPT\*\*  

Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter



\---



> \*"Every bug has a cause. Every algorithm has a story. Every dragon can be tamed."\*

