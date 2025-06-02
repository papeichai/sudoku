#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <string>
#include <ctime>  

using namespace std;

class Sudoku {
private:
    vector<vector<int>> board;
    vector<vector<int>> solution;
    vector<vector<bool>> fixed;
    int emptyCells;
    int cursorRow, cursorCol;
    string lastError;

    bool isValid(int row, int col, int num) {
        // Проверка строки и столбца
        for (int i = 0; i < 9; ++i) {
            if (board[row][i] == num || board[i][col] == num) {
                return false;
            }
        }

        // Проверка квадрата 3x3
        int boxRow = row - row % 3;
        int boxCol = col - col % 3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[boxRow + i][boxCol + j] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    bool solveSudoku(int row, int col) {
        if (row == 9) return true;
        if (col == 9) return solveSudoku(row + 1, 0);
        if (board[row][col] != 0) return solveSudoku(row, col + 1);

        for (int num = 1; num <= 9; ++num) {
            if (isValid(row, col, num)) {
                board[row][col] = num;
                if (solveSudoku(row, col + 1)) return true;
                board[row][col] = 0;
            }
        }
        return false;
    }

    void fillDiagonalBoxes() {
        vector<int> nums = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
        shuffle(nums.begin(), nums.end(), default_random_engine(seed));

        for (int box = 0; box < 3; ++box) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    board[box * 3 + i][box * 3 + j] = nums[i * 3 + j];
                }
            }
            shuffle(nums.begin(), nums.end(), default_random_engine(seed + box + 1));
        }
    }

    int countSolutions(int row, int col) {
        if (row == 9) return 1;
        if (col == 9) return countSolutions(row + 1, 0);
        if (board[row][col] != 0) return countSolutions(row, col + 1);

        int count = 0;
        for (int num = 1; num <= 9 && count < 2; ++num) {
            if (isValid(row, col, num)) {
                board[row][col] = num;
                count += countSolutions(row, col + 1);
                board[row][col] = 0;
            }
        }
        return count;
    }

    void removeNumbers(int difficulty) {
        int cellsToRemove;
        switch (difficulty) {
        case 1: cellsToRemove = 30; break;
        case 2: cellsToRemove = 40; break;
        case 3: cellsToRemove = 50; break;
        default: cellsToRemove = 35; break;
        }

        emptyCells = cellsToRemove;

        while (cellsToRemove > 0) {
            int row = rand() % 9;
            int col = rand() % 9;

            if (board[row][col] != 0) {
                int temp = board[row][col];
                board[row][col] = 0;

                if (countSolutions(0, 0) == 1) {
                    fixed[row][col] = false;
                    cellsToRemove--;
                }
                else {
                    board[row][col] = temp;
                }
            }
        }

        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] != 0) {
                    fixed[i][j] = true;
                }
            }
        }
    }

public:
    Sudoku(int difficulty = 2) {
        srand(static_cast<unsigned>(time(nullptr)));
        board = vector<vector<int>>(9, vector<int>(9, 0));
        fixed = vector<vector<bool>>(9, vector<bool>(9, false));
        cursorRow = cursorCol = 0;
        emptyCells = 0;
        lastError = "";
        fillDiagonalBoxes();
        solveSudoku(0, 0);
        solution = board;
        removeNumbers(difficulty);
    }

    void printBoard() {
        system("cls");

        cout << "=== СУДОКУ === (Управление: стрелки, 1-9 - ввод, 0 - очистить)\n";
        cout << "   A   B   C    D   E   F    G   H   I\n";
        cout << "  +-----------+-----------+-----------+\n";

        for (int i = 0; i < 9; ++i) {
            cout << " " << i + 1 << "| ";
            for (int j = 0; j < 9; ++j) {
                if (i == cursorRow && j == cursorCol) {
                    cout << "[";
                    if (board[i][j] == 0) cout << " ";
                    else cout << board[i][j];
                    cout << "]";
                }
                else {
                    cout << " ";
                    if (board[i][j] == 0) cout << " ";
                    else cout << board[i][j];
                    cout << " ";
                }

                if (j == 2 || j == 5) {
                    cout << " | ";
                }
            }
            cout << "|\n";

            if (i == 2 || i == 5) {
                cout << "  +-----------+-----------+-----------+\n";
            }
        }
        cout << "  +-----------+-----------+-----------+\n";
        cout << "Осталось заполнить: " << emptyCells << " клеток\n";

        if (!lastError.empty()) {
            cout << "\nОшибка: " << lastError << "\n";
            lastError = "";
        }
    }

    void moveCursor(int direction) {
        switch (direction) {
        case 72: if (cursorRow > 0) cursorRow--; break; // Вверх
        case 80: if (cursorRow < 8) cursorRow++; break; // Вниз
        case 75: if (cursorCol > 0) cursorCol--; break; // Влево
        case 77: if (cursorCol < 8) cursorCol++; break; // Вправо
        }
    }

    bool makeMove(int num) {
        if (fixed[cursorRow][cursorCol]) {
            lastError = "Эта клетка фиксирована!";
            printBoard();
            (void)_getch();
            return false;
        }

        if (num < 1 || num > 9) {
            lastError = "Число должно быть от 1 до 9!";
            printBoard();
            (void)_getch();
            return false;
        }

        if (!isValid(cursorRow, cursorCol, num)) {
            lastError = "Это число нарушает правила!";
            printBoard();
            (void)_getch();
            return false;
        }

        if (board[cursorRow][cursorCol] == 0) emptyCells--;
        board[cursorRow][cursorCol] = num;
        return true;
    }

    bool clearCell() {
        if (fixed[cursorRow][cursorCol]) {
            lastError = "Эта клетка фиксирована!";
            printBoard();
            (void)_getch();
            return false;
        }

        if (board[cursorRow][cursorCol] != 0) {
            emptyCells++;
            board[cursorRow][cursorCol] = 0;
            return true;
        }
        return false;
    }

    void showHint() {
        if (board[cursorRow][cursorCol] == 0) {
            cout << "Подсказка: " << cursorRow + 1 << char('A' + cursorCol)
                << " = " << solution[cursorRow][cursorCol] << "\n";
        }
        else {
            cout << "Клетка уже заполнена!\n";
        }
    }

    void checkErrors() {
        system("cls");
        printBoard();

        int errors = 0;
        cout << "\nОшибки в клетках: ";
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] != 0 && board[i][j] != solution[i][j]) {
                    errors++;
                    cout << i + 1 << char('A' + j) << " ";
                }
            }
        }
        if (errors == 0) {
            cout << "Ошибок нет!\n";
        }
        else {
            cout << "\nВсего ошибок: " << errors << "\n";
        }
    }

    void showSolution() {
        cout << "Решение:\n";
        cout << "     A B C   D E F   G H I\n";
        cout << "   +-------+-------+-------+\n";
        for (int i = 0; i < 9; ++i) {
            cout << i + 1 << " | ";
            for (int j = 0; j < 9; ++j) {
                cout << solution[i][j] << " ";
                if (j == 2 || j == 5) cout << "| ";
            }
            cout << "|\n";

            if (i == 2 || i == 5) {
                cout << "   +-------+-------+-------+\n";
            }
        }
        cout << "   +-------+-------+-------+\n";
    }

    bool isSolved() const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] != solution[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    cout << "=== ИГРА СУДОКУ ===\n";
    cout << "Выберите сложность (1-легко, 2-средне, 3-сложно): ";
    int difficulty;
    cin >> difficulty;
    cin.ignore();

    Sudoku game(difficulty);

    while (true) {
        game.printBoard();
        cout << "\nУправление:\n";
        cout << "  Стрелки - перемещение курсора\n";
        cout << "  1-9 - ввести число\n";
        cout << "  0 - очистить клетку\n";
        cout << "  H - подсказка\n";
        cout << "  C - проверить ошибки\n";
        cout << "  S - показать решение\n";
        cout << "  N - новая игра\n";
        cout << "  Q - выход\n";

        int key = _getch();

        if (key == 224) { // Специальные клавиши
            key = _getch();
            game.moveCursor(key);
        }
        else if (key >= '1' && key <= '9') {
            if (!game.makeMove(key - '0')) {
                continue;
            }
        }
        else if (key == '0') {
            if (!game.clearCell()) {
                continue;
            }
        }
        else {
            switch (toupper(key)) {
            case 'H':
                game.showHint();
                cout << "Нажмите любую клавишу...";
                (void)_getch();
                break;
            case 'C':
                game.checkErrors();
                cout << "\nНажмите любую клавишу для продолжения...";
                (void)_getch();
                break;
            case 'S':
                game.showSolution();
                cout << "Нажмите любую клавишу для выхода...";
                (void)_getch();
                return 0;
            case 'N':
                cout << "Новая игра. Выберите сложность (1-3): ";
                cin >> difficulty;
                cin.ignore();
                game = Sudoku(difficulty);
                break;
            case 'Q':
                return 0;
            default:
                break;
            }
        }

        if (game.isSolved()) {
            game.printBoard();
            cout << "\nПОЗДРАВЛЯЕМ! Вы решили судоку!\n";
            cout << "Нажмите любую клавишу для выхода...";
            (void)_getch();
            break;
        }
    }

    return 0;
}