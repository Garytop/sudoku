// traditional_solver.cpp
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace chrono;

class SudokuSolver {
private:
  vector<vector<int>> board;
  bool found = false;

  bool isValid(int row, int col, int num) {
    for (int c = 0; c < 9; c++)
      if (board[row][c] == num)
        return false;

    for (int r = 0; r < 9; r++)
      if (board[r][col] == num)
        return false;

    int startRow = 3 * (row / 3);
    int startCol = 3 * (col / 3);
    for (int r = 0; r < 3; r++)
      for (int c = 0; c < 3; c++)
        if (board[startRow + r][startCol + c] == num)
          return false;
    return true;
  }

  pair<int, int> findEmpty() {
    for (int r = 0; r < 9; r++)
      for (int c = 0; c < 9; c++)
        if (board[r][c] == 0)
          return {r, c};
    return {-1, -1};
  }

public:
  SudokuSolver(const vector<vector<int>> &input) : board(input) {}

  bool solve() {
    auto [row, col] = findEmpty();
    if (row == -1)
      return true;

    for (int num = 1; num <= 9; num++) {
      if (isValid(row, col, num)) {
        board[row][col] = num;
        if (solve())
          return true;
        board[row][col] = 0;
      }
    }
    return false;
  }

  string getSolution() {
    stringstream ss;
    for (auto &row : board)
      for (int num : row)
        ss << num;
    return ss.str();
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <sudoku_string>" << endl;
    return 1;
  }

  string input = argv[1];
  vector<vector<int>> board(9, vector<int>(9));

  for (int i = 0; i < 81; i++)
    board[i / 9][i % 9] = input[i] - '0';

  auto start = high_resolution_clock::now();
  SudokuSolver solver(board);
  bool solved = solver.solve();
  auto end = high_resolution_clock::now();

  if (solved) {
    cout << solver.getSolution() << " "
         << duration_cast<microseconds>(end - start).count() / 1e6 << endl;
  } else {
    cout << "No solution" << endl;
  }
  return 0;
}
