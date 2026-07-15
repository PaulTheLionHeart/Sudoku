# Sudoku Help

Welcome to Sudoku.

This application preserves the original Sudoku solving engine while providing a modern x64 Win32 interface for both classic 9×9 Sudoku and 16×16 hexadecimal Sudoku.

The program has been carefully modernised while preserving the simplicity, reliability and character of the original application.

---

# Contents

1. Welcome
2. A Quick Demonstration
3. The Main Window
4. Creating a Puzzle
5. Solving a Puzzle
6. Working with 9×9 Sudoku
7. Working with 16×16 Sudoku
8. Saving and Loading
9. Tips
10. Behind the Scenes
11. FireEater's Corner

---

# Welcome

Sudoku supports two puzzle sizes:

- Classic 9×9 Sudoku
- 16×16 Hexadecimal Sudoku

Both puzzle types use the same solving engine.

Only the user interface differs.

---

# A Quick Demonstration

## Step 1 – Select the Puzzle Type

Press **S** or click **Select**.

Choose either

- 9×9 Sudoku
- 16×16 Hexadecimal Sudoku

The default is 9×9.

---

## Step 2 – Create a New Puzzle

Press **C** or click **Create**.

A blank puzzle is displayed.

The **Original Puzzle** option is enabled automatically.

---

## Step 3 – Enter the Original Clues

Enter the original puzzle exactly as published.

Original clues are displayed in **red**.

### 9×9

Click one of the candidate buttons.

### 16×16

Click a cell.

Select the hexadecimal value from the value picker.

---

## Step 4 – Begin Solving

When all original clues have been entered:

**Turn OFF Original Puzzle.**

All subsequent entries become user values and are displayed in black.

---

## Step 5 – Solve

Press **Solve**.

The program completes the puzzle automatically.

If no valid solution exists, an **Impasse** message is displayed.

---

## Step 6 – Save

Click **Save Sudoku**.

Your puzzle can now be reopened later.

---

# The Main Window

The main window contains the following controls.

## Select

Choose between 9×9 and 16×16 Sudoku.

## Create

Create a new blank puzzle.

## Original Puzzle

Determines whether newly entered values are treated as original clues.

Original clues appear in red.

User entries appear in black.

## Solve

Runs the built-in Sudoku solver.

## Refresh

Clears the current puzzle.

## Save Sudoku

Saves the puzzle using the native Sudoku format.

## Save CSV

Exports the puzzle as comma-separated values.

## Finish

Closes the Sudoku editor.

---

# Creating a Puzzle

The recommended workflow is

1. Create a blank puzzle.
2. Leave Original Puzzle checked.
3. Enter all original clues.
4. Turn Original Puzzle OFF.
5. Begin solving.

If an incorrect value is entered,

left-click the completed cell to remove it.

Right-click any completed cell to toggle between Original and User colours.

---

# Solving a Puzzle

The Sudoku solver uses the same solving engine for both puzzle sizes.

Press **Solve** whenever you wish the program to complete the puzzle automatically.

If an **Impasse** message appears,

the current puzzle contains inconsistent information.

Check your entries before attempting to solve again.

---

# Working with 9×9 Sudoku

Each cell contains

- one large value button
- nine clickable candidate buttons

Simply click the required candidate.

Undo is performed by clicking a completed cell.

---

# Working with 16×16 Sudoku

Values are hexadecimal.

The digits are

```
0 1 2 3 4 5 6 7 8 9 A B C D E F
```

Each cell displays painted candidates.

Clicking a cell opens the hexadecimal value picker.

This approach provides a much cleaner interface than attempting to display sixteen tiny buttons inside every cell.

---

# Saving and Loading

Two file formats are supported.

## Sudoku

The native editable puzzle format.

## CSV

A spreadsheet-compatible export.

Completed puzzles may be saved and reloaded.

Reloaded values become editable entries.

---

# Tips

- Save frequently.
- Turn off Original Puzzle before solving.
- Right-click toggles Original/User colours.
- Left-click removes completed values.
- Use Solve whenever you wish the computer to finish the puzzle.

---

# Behind the Scenes

Both puzzle sizes use exactly the same solving engine.

Only the presentation differs.

9×9 uses clickable candidate buttons because they provide the simplest interface.

16×16 uses painted candidates and a hexadecimal value picker because displaying sixteen candidate buttons inside every cell would create over four thousand tiny controls.

The goal of the project has always been to preserve the proven solving engine while modernising the architecture and user interface.

---

# FireEater's Corner 🐉

Welcome!

I'm FireEater,

Chief Bug Inspector for the LionHeart Workshop.

Contrary to certain rumours,

I no longer eat source code.

Much.

## FireEater's Workshop Rules

- Every bug has a cause.
- Every algorithm has a story.
- Every dragon leaves a trail.
- Follow the value, not the symptoms.
- Never assume. Verify.
- Good software isn't rewritten. It's understood.
- Coffee first. Refactoring second.

## FireEater's Tips

> If Impasse appears, don't blame me immediately.

> Every dragon leaves a trail.

> Follow the value, not the symptoms.

> Bugs are much easier to catch before midnight.

---

# Credits

**Paul the LionHeart**

Author and Developer

**ChatGPT**

Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter

---

## Acknowledgements

**FireEater** 🐉  
Chief Bug Inspector  
(Retired from Code Consumption... Mostly.)

**The Friendly Workshop Dragons** 🐉🐉🐉  
Providers of encouragement, comic relief, and occasional bug detection.

---

*"Fun, beauty and passion."*