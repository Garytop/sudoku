// threaded_solver.cpp
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;
using namespace chrono;

class ThreadedSolver {
private:
  vector<vector<int>> initial_board;
  vector<vector<int>> solution;
  atomic<bool> found{false};
  mutex mtx;

  bool isValid(const vector<vector<int>> &b, int row, int col, int num) {
    for (int c = 0; c < 9; c++)
      if (b[row][c] == num)
        return false;

    for (int r = 0; r < 9; r++)
      if (b[r][col] == num)
        return false;

    int startRow = 3 * (row / 3);
    int startCol = 3 * (col / 3);
    for (int r = 0; r < 3; r++)
      for (int c = 0; c < 3; c++)
        if (b[startRow + r][startCol + c] == num)
          return false;
    return true;
  }

  pair<int, int> findEmpty(const vector<vector<int>> &b) {
    for (int r = 0; r < 9; r++)
      for (int c = 0; c < 9; c++)
        if (b[r][c] == 0)
          return {r, c};
    return {-1, -1};
  }

  void solveRecursive(vector<vector<int>> b) {
    if (found)
      return;

    auto [row, col] = findEmpty(b);
    if (row == -1) {
      lock_guard<mutex> lock(mtx);
      if (!found) {
        solution = b;
        found = true;
      }
      return;
    }

    for (int num = 1; num <= 9; num++) {
      if (found)
        return;
      if (isValid(b, row, col, num)) {
        b[row][col] = num;
        solveRecursive(b);
        b[row][col] = 0;
      }
    }
  }

public:
  ThreadedSolver(const vector<vector<int>> &input) : initial_board(input) {}

  bool solve() {
    auto [row, col] = findEmpty(initial_board);
    if (row == -1) {
      solution = initial_board;
      return true;
    }

    vector<thread> threads;
    for (int num = 1; num <= 9; num++) {
      if (found)
        break;
      if (isValid(initial_board, row, col, num)) {
        auto newBoard = initial_board;
        newBoard[row][col] = num;
        threads.emplace_back(&ThreadedSolver::solveRecursive, this, newBoard);
      }
    }

    for (auto &t : threads)
      if (t.joinable())
        t.join();

    return found;
  }

  string getSolution() {
    stringstream ss;
    for (auto &row : solution)
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
  ThreadedSolver solver(board);
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
