// 多线程数独求解器
// 为每个列分配一个线程，为每个列分配一个线程，为每个宫格分配一个线程
// 总共有27个线程，父线程等待子线程结束，通过全局flag数组来判定解是否合理

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

bool flag[27];    // 27个线程的标志位
int sudoku[9][9]; // 数独矩阵

// check row
void *check_row(void *arg) {
  int row = *(int *)arg;
  bool used[10] = {false}; // 1-9的数字是否使用过
  for (int col = 0; col < 9; col++) {
    if (sudoku[row][col] != 0) {
      if (used[sudoku[row][col]]) {
        flag[row] = false; // 行重复
        pthread_exit(NULL);
      }
      used[sudoku[row][col]] = true;
    } else {
      flag[row] = false; // 行有空格
      pthread_exit(NULL);
    }
  }
  flag[row] = true; // 行合法
  pthread_exit(NULL);
}

// check column
void *check_col(void *arg) {
  int col = *(int *)arg;
  bool used[10] = {false}; // 1-9的数字是否使用过
  for (int row = 0; row < 9; row++) {
    if (sudoku[row][col] != 0) {
      if (used[sudoku[row][col]]) {
        flag[col + 9] = false; // 列重复
        pthread_exit(NULL);
      }
      used[sudoku[row][col]] = true;
    } else {
      flag[col + 9] = false; // 列有空格
      pthread_exit(NULL);
    }
  }
  flag[col + 9] = true; // 列合法
  pthread_exit(NULL);
}

// check block
void *check_block(void *arg) {
  int block = *(int *)arg;
  bool used[10] = {false}; // 1-9的数字是否使用过
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int r = (block / 3) * 3 + row;
      int c = (block % 3) * 3 + col;
      if (sudoku[r][c] != 0) {
        if (used[sudoku[r][c]]) {
          flag[block + 18] = false; // 宫格重复
          pthread_exit(NULL);
        }
        used[sudoku[r][c]] = true;
      } else {
        flag[block + 18] = false; // 宫格有空格
        pthread_exit(NULL);
      }
    }
  }
  flag[block + 18] = true; // 宫格合法
  pthread_exit(NULL);
}

// check sudoku
void *check_sudoku(void *arg) {
  pthread_t threads[27];
  int args[27];
  for (int i = 0; i < 9; i++) {
    args[i] = i;
    pthread_create(&threads[i], NULL, check_row, (void *)&args[i]);
  }
  for (int i = 0; i < 9; i++) {
    args[i + 9] = i;
    pthread_create(&threads[i + 9], NULL, check_col, (void *)&args[i + 9]);
  }
  for (int i = 0; i < 9; i++) {
    args[i + 18] = i;
    pthread_create(&threads[i + 18], NULL, check_block, (void *)&args[i + 18]);
  }
  for (int i = 0; i < 27; i++) {
    pthread_join(threads[i], NULL);
  }
  return NULL;
}

bool check_valid() {
  for (int i = 0; i < 27; i++) {
    if (!flag[i]) {
      return false;
    }
  }
  return true;
}

bool input_data() {
  printf("请输入数独矩阵：\n");
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      scanf("%d", &sudoku[i][j]);
    }
  }
  return true;
}

bool input_data_from_file() {
  FILE *fp = fopen("sudoku.txt", "r");
  if (fp == NULL) {
    printf("打开文件失败\n");
    return false;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      fscanf(fp, "%d", &sudoku[i][j]);
    }
  }
  fclose(fp);
  return true;
}

void menu() {
  printf("请选择输入方式：\n");
  printf("1. 从文件输入\n");
  printf("2. 从控制台输入\n");
  printf("请输入选择(默认为1):");
}

void show_data() {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      printf("%d ", sudoku[i][j]);
    }
    printf("\n");
  }
}

int main() {
  menu();
  char choice;
  scanf("%c", &choice);
  if (choice == '1' || choice != '2') {
    choice = 1;
  } else {
    choice = 2;
  }
  if (choice == 1) {
    input_data_from_file();
  } else {
    input_data();
  }
  show_data();

  // 检查数独
  check_sudoku(NULL);
  // 输出结果
  bool valid = check_valid();
  if (valid) {
    printf("数独合法\n");
  } else {
    printf("数独不合法\n");
  }

  return 0;
}
