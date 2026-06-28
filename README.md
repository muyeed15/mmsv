# MMSV - Multithreaded Magic Square Validator

A program that reads a square matrix from a file and checks if it is a magic square using 4 parallel threads.

## What is a Magic Square

Given an n x n grid of numbers 1 to n^2. The magic constant is:

```
M = n * (n^2 + 1) / 2
```

For example, a 3x3 grid uses numbers 1 to 9. The magic constant is `M = 3 * (9 + 1) / 2 = 15`.

For the grid to be valid, all of these must sum to M:
- Sum of every row
- Sum of every column
- Sum of the main diagonal (top-left to bottom-right)
- Sum of the secondary diagonal (top-right to bottom-left)
- Must contain each number 1 to n^2 exactly once (no duplicates, no missing)

## How the Program Works

4 threads run at the same time:
- Thread 1: adds up each row, checks if it equals M
- Thread 2: adds up each column, checks if it equals M
- Thread 3: adds up both diagonals, checks if they equal M
- Thread 4: checks that numbers 1 to n^2 each appear exactly once

Each check that passes adds +1 to a shared score. Each that fails subtracts 1. The score is protected by a mutex so only one thread touches it at a time.

The max possible score is `2n + 3` (n rows + n columns + 2 diagonals + 1 uniqueness check). The grid is valid only if the final score equals the max.

## Build and Run

```
make
./mmsv test_valid_3x3.txt
```

## Example

Input file (`test_valid_3x3.txt`):
```
3
8 1 6
3 5 7
4 9 2
```

Row sums: 8+1+6=15, 3+5+7=15, 4+9+2=15 (all match M=15)

Column sums: 8+3+4=15, 1+5+9=15, 6+7+2=15 (all match)

Diagonals: 8+5+2=15, 6+5+4=15 (both match)

Uniqueness: contains 1 through 9, no duplicates

Output:
```
Thread ID-1: Row checks completed.
Thread ID-3: Diagonal checks completed.
Thread ID-4: Uniqueness check completed.
Thread ID-2: Column checks completed.

--- Magic Square Report ---
Rows: All Valid
Cols: All Valid
Diags: All Valid
Unique: Passed
Final Score: 9 / 9
RESULT: VALID MAGIC SQUARE
```

## Test Files

```
./mmsv test_valid_3x3.txt      # VALID 3x3
./mmsv test_invalid_3x3.txt    # INVALID 3x3 (duplicate value)
./mmsv test_valid_4x4.txt      # VALID 4x4
./mmsv test_invalid_4x4.txt    # INVALID 4x4 (duplicate value)
```

## Clean

```
make clean
```
