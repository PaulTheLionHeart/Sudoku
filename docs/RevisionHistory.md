# Revision History

This document summarises the evolution of the Sudoku project.

---

## Version 1.1 – Sudoku Generator

Release date: July 2026

### New Features

- Sudoku puzzle generator.
- Automatic generation of both 9×9 and 16×16 puzzles.
- Unique-solution verification.
- Difficulty selection.
- Puzzle symmetry options:
  - Random
  - Rotational
  - Mirror
- Generation statistics.
- Progress reporting during generation.
- Integrated generator for both classic 9×9 and hexadecimal 16×16 Sudoku.

### Improvements

- Improved generator architecture.
- Better code documentation throughout the project.
- Enhanced help documentation.
- Updated README.
- New Revision History document.

### Internal Improvements

- Minimum Remaining Values (MRV) heuristic shared by both solver and generator.
- Shared helper functions.
- Cleaner separation between generator, solver and display code.
- Improved comments describing design decisions.

---

## Version 1.0a – Documentation Update

This update focuses on improving the user experience and expanding the project documentation.

### Improvements

- Renamed Calculate to Solve for terminology that better reflects Sudoku solving.
- Expanded the project documentation.
- Added a comprehensive User Guide.
- Added a Quick Reference guide.
- Improved the project README.
- Added documentation links from the README.
- Continued refinement of the modern 9×9 and 16×16 interfaces.

### Documentation

The project now includes:

- User Guide
- Quick Reference
- Improved project overview
- Credits and acknowledgements

---

## Version 1.0 – Initial Public Release

This is the first public release of the modernised Sudoku project.

The application preserves the original, well-tested Sudoku solving engine while introducing a modern x64 Win32 codebase with a cleaner architecture and support for both classic and hexadecimal Sudoku.

### Features

- Native x64 application
- Classic 9×9 Sudoku
- 16×16 hexadecimal Sudoku
- Interactive candidate display
- Mouse-based puzzle editing
- Original/User clue editing
- Undo support
- Save and reload puzzles
- Shared solving engine for both board sizes

### Modernisation

- Modern Win32 callback signatures
- Legacy Win16 declarations removed
- Board rendering separated into SudokuDisplay.cpp
- Cleaner separation between application logic and display logic
- Simplified editing workflow
- Separate rendering paths for 9×9 and 16×16 boards
- Painted hexadecimal candidate display
- Interactive hexadecimal value picker
- Numerous bug fixes and code clean-up

### Credits

Paul the LionHeart
Author and Developer

ChatGPT
Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter

---

The LionHeart Workshop believes that mature software should be modernised thoughtfully rather than rewritten unnecessarily.

Each release focuses on making the program cleaner, easier to understand and more enjoyable to use while preserving the reliability of the original algorithms.

"Fun, beauty and passion."

"Every dragon leaves a trail."


