#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

using namespace std;

class SudokuSolver {
private:
    vector<vector<int>> initial_board;
    vector<vector<int>> solution;
    atomic<bool> found;
    mutex mtx;

    // 检查数字有效性
    bool is_valid(const vector<vector<int>>& board, int row, int col, int num) {
        // 检查行
        for (int c = 0; c < 9; ++c) {
            if (board[row][c] == num) return false;
        }
        
        // 检查列
        for (int r = 0; r < 9; ++r) {
            if (board[r][col] == num) return false;
        }
        
        // 检查3x3子网格
        int start_row = 3 * (row / 3);
        int start_col = 3 * (col / 3);
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board[start_row + r][start_col + c] == num) {
                    return false;
                }
            }
        }
        return true;
    }

    // 寻找第一个空白格
    pair<int, int> find_empty(const vector<vector<int>>& board) {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (board[r][c] == 0) {
                    return {r, c};
                }
            }
        }
        return {-1, -1}; // 无空白格
    }

    // 获取候选数字
    vector<int> get_possible_numbers(const vector<vector<int>>& board, int row, int col) {
        vector<int> candidates;
        for (int num = 1; num <= 9; ++num) {
            if (is_valid(board, row, col, num)) {
                candidates.push_back(num);
            }
        }
        return candidates;
    }

    // 单线程回溯求解
    bool solve_single(vector<vector<int>>& board) {
        if (found) return false;

        auto pos = find_empty(board);
        if (pos.first == -1) {
            return true; // 找到解
        }

        int row = pos.first;
        int col = pos.second;

        vector<int> candidates = get_possible_numbers(board, row, col);
        for (int num : candidates) {
            if (found) return false;

            board[row][col] = num;
            if (solve_single(board)) {
                return true;
            }
            board[row][col] = 0; // 回溯
        }
        return false;
    }

    // 工作线程函数
    void worker(vector<vector<int>> board) {
        if (solve_single(board) && !found) {
            lock_guard<mutex> lock(mtx);
            if (!found) {
                solution = board;
                found = true;
            }
        }
    }

public:
    SudokuSolver(const vector<vector<int>>& board) 
        : initial_board(board), found(false) {}

    bool solve() {
        auto first_empty = find_empty(initial_board);
        if (first_empty.first == -1) {
            solution = initial_board;
            return true;
        }

        int row = first_empty.first;
        int col = first_empty.second;
        vector<int> candidates = get_possible_numbers(initial_board, row, col);

        vector<thread> threads;
        for (int num : candidates) {
            if (found) break;

            vector<vector<int>> new_board = initial_board;
            new_board[row][col] = num;

            threads.emplace_back(&SudokuSolver::worker, this, new_board);
        }

        for (auto& t : threads) {
            t.join();
        }

        return found;
    }

    void print_solution() {
        if (solution.empty()) {
            cout << "No solution found" << endl;
            return;
        }

        for (const auto& row : solution) {
            for (int num : row) {
                cout << (num ? to_string(num) : ".") << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    vector<vector<int>> sudoku_board = {
        {5,3,0,0,7,0,0,0,0},
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };

    SudokuSolver solver(sudoku_board);
    
    auto start = chrono::high_resolution_clock::now();
    bool solved = solver.solve();
    auto end = chrono::high_resolution_clock::now();

    if (solved) {
        chrono::duration<double> elapsed = end - start;
        cout << "Solution found in " << elapsed.count() << " seconds" << endl;
        solver.print_solution();
    } else {
        cout << "No solution exists" << endl;
    }

    return 0;
}