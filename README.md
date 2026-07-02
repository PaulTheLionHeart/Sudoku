# Sudoku

A modernisation of a classic Win32 Sudoku program.

The aim of this project is to preserve the original solver while carefully
modernising the user interface, architecture and codebase without rewriting
the application from scratch.

## Status

⚠️ **This repository is currently under active development.**

The project now builds cleanly as a modern x64 Win32 application.

The original Sudoku engine is working, but the user interface is currently
being redesigned. At present, the application should be regarded as
**experimental** rather than a finished Sudoku program.

The long-term goal is to replace the original button-based board with a
renderer while preserving the existing Sudoku engine.

## Current Features

- Native x64 build
- Modern Win32 callback signatures
- Legacy Win16 declarations removed
- Visual Studio 2017 project
- Original backtracking Sudoku solver
- Support for both 9×9 and 16×16 Sudoku

## Planned Improvements

- Renderer-based Sudoku board
- Simplified UI architecture
- Candidate helper functions
- Keyboard input for 16×16 puzzles
- Improved project organisation
- CMake build system

## Philosophy

This project is intended to demonstrate that legacy Win32 software can be
carefully modernised one step at a time without losing the reliability of the
original program.

Rather than rewriting everything from scratch, each change is small,
well-tested and designed to leave the codebase cleaner than before.