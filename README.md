# Sudoku



A modernisation of a classic Win32 Sudoku program.



The aim of this project is to preserve the original solving engine while carefully modernising the user interface, architecture and codebase without rewriting the application from scratch.



## Status

**Current Release:** Sudoku 1.1

Sudoku is a native x64 Win32 application supporting both classic **9×9** and **16×16 hexadecimal Sudoku**.

Version 1.1 introduces an integrated puzzle generator together with continued improvements to the architecture, documentation and user interface.


## Documentation

The complete user manual is supplied with the application.

Quick links:

- 📖 [Sudoku Help](docs/UserGuide.md) – Complete user guide.
- ⚡ [Quick Reference](docs/QuickReference.md) – Keyboard shortcuts and common tasks.
- 📜 [Revision History](docs/RevisionHistory.md) – Evolution of the project.

The documentation explains both the classic 9×9 interface and the modern 16×16 hexadecimal interface, together with the design philosophy behind the project.


## Current Features



- Native x64 build
- Visual Studio 2017 project
- Modern Win32 callback signatures
- Legacy Win16 declarations removed
- Original backtracking Sudoku solver
- 9×9 and 16×16 puzzle generation
- Interactive candidate display
- Mouse-based puzzle editing
- Undo support
- Right-click Original/User clue toggling
- Shared board helper architecture


## Sudoku Generator

Version 1.1 adds an integrated Sudoku puzzle generator capable of generating both 9×9 and 16×16 puzzles.

The generator first creates a complete Sudoku grid before removing clues while continually verifying that the puzzle retains a unique solution.

Features include:

- 9×9 and 16×16 puzzle generation
- Difficulty selection
- Random, Rotational and Mirror symmetry
- Progress reporting
- Generation statistics

## Design Philosophy

Rather than relying on pre-generated puzzles, Sudoku generates each puzzle on demand.

Every generated puzzle is verified to have a unique solution before it is presented to the user, ensuring that each game is both valid and solvable.

## Recent Modernisation


- Board display logic separated into `SudokuDisplay.cpp`
- Cleaner separation between application logic and board display logic
- Simplified cell-selection and value-placement workflow
- Separate rendering paths for 9×9 and 16×16 boards
- Painted 4×4 hexadecimal candidate display
- Clickable hexadecimal value picker
- Integrated Sudoku puzzle generator with unique-solution verification
- Shared solver and generator architecture using the Minimum Remaining Values (MRV) heuristic
- Modern helper functions for board operations
- Simplified dialog architecture with obsolete legacy flow removed
- Improved readability and maintainability throughout the codebase


## Board Design



The two board sizes intentionally use different user interfaces while sharing the same solving engine.



### 9×9 Sudoku



Each cell retains the original interactive design:



- One large value button
- Nine clickable candidate buttons
- Direct candidate selection
- Red original clues and black user entries



### 16×16 Hexadecimal Sudoku



Each cell uses one large control rather than sixteen tiny candidate buttons:



- Candidates are painted directly in a 4×4 layout
- Clicking a cell opens the hexadecimal value picker
- Values are displayed using `0–9` and `A–F`
- Right-clicking a filled cell toggles its Original/User colour
- Candidate availability is calculated directly from the Sudoku engine

This avoids the impractical alternative of creating 4096 tiny candidate controls.



## Editing Behaviour



Filled cells are tracked using three separate pieces of information:



- `matrix\[]\[]` stores the value
- `known\[]\[]` identifies cells fixed before solving
- `orig\[]\[]` controls whether a value is displayed as an original clue or a user entry



Original clues are displayed in red, while user-entered and solver-generated values are displayed in black.



Solver-generated values are intentionally not marked as `known`, because the backtracking solver must remain free to clear and replace them while searching for a solution.



When a completed puzzle is saved and reloaded, all loaded values become fixed entries and may then be toggled between Original and User status.


## Planned Improvements



- Additional generation options
- Improved difficulty analysis
- Continued code cleanup
- Optional CMake build support


## Philosophy



This project demonstrates that mature Win32 software can be modernised incrementally while preserving the reliability of its original algorithms.



Rather than rewriting decades of proven code, each improvement is carefully designed, tested and integrated so that the codebase becomes progressively cleaner, simpler and easier to understand.



The different board presentations are deliberate. Each board size receives the interface best suited to it without compromising the shared solving architecture.



---


## Credits

**Paul the LionHeart**

Author and Developer

**ChatGPT**

Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter

---

> *"Follow the value, not the symptoms. Every dragon leaves a trail."*