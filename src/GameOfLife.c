#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#define M 25  // число строк мира
#define N 80  // число столбцов мира
#define stringborder 43  // вид границы поля
#define life 111  // вид отоброжения жизни
#define death 32  // вид умершей клетки клетки
#define worldPiece 32  // вид пустой клетки
#define goNextDay 32  // клавиша перехода на следующий день
#define exitWorld 'q'  // клавиша завершения игры

// считывает начальное положения мира из стандартного потока ввода
int DoManualInput(char data[][N + 2]);
// реализует переход от одного дня к другому
void DoEvolution();
// генерирует границы мира [N+2][M+2] (с учётом рамки)
void MakeGameSpace(char data[][N + 2]);
// принимает элемент массива, возвращает количество живых клеток вокруг
int LifeCounter(char data[][N + 2], int i, int j);
// изменяет элементы массива следующего дня в зависимости от условий выживания
int MakeLife(char data_1[][N + 2], char data_2[][N + 2]);
// рисует мир
void PrintTheWorld(char data[][N + 2]);
// копирует состояние одного дня в другой
void CopyDay(char data_1[][N + 2], char data_2[][N + 2]);
// принимает две матрицы. если они отличаются - возвращает 1, если нет - 0
int CompareDays(char data_1[][N + 2], char data_2[][N + 2]);
// блокирует сигналы
void StopSignals();

static struct termios stored_settings;
void set_keypress(void);
void reset_keypress(void);

int main() {
    DoEvolution();
    return 0;
}

void DoEvolution() {
    StopSignals();
    int correctInput;
    char step;
    char gameSpace[M + 2][N + 2];
    MakeGameSpace(gameSpace);
    correctInput = DoManualInput(gameSpace);
    if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("Перенаправить поток ввода не удалось\n");
    }
    if (correctInput == 0) {
    char nextDay[M + 2][N + 2];
    MakeGameSpace(nextDay);
    int lifePoint, evolutionPoint;
    PrintTheWorld(gameSpace);
    set_keypress();
        do {
            scanf("%c", &step);
            if (step == goNextDay) {
                lifePoint = MakeLife(gameSpace, nextDay);
                if (lifePoint == 1) {
                    PrintTheWorld(nextDay);
                    printf("Колония мертва\n");
                    break;
                }
                evolutionPoint = CompareDays(gameSpace, nextDay);
                if (evolutionPoint == 1) {
                    PrintTheWorld(nextDay);
                    printf("Колония стабильна и не развивается(\n");
                    break;
                }
                CopyDay(gameSpace, nextDay);
                PrintTheWorld(gameSpace);
                MakeGameSpace(nextDay);
            }
        } while (step != exitWorld);
        reset_keypress();
    }
}

int DoManualInput(char data[][N + 2]) {
    int correct = 1;
    char meaning;
    for (int m = 1; m < M + 1; m++) {
        for (int n = 1; n < N + 1; n++) {
            scanf("%c", &meaning);
            if (meaning == 48) {
                data[m][n] = worldPiece;
                correct = 0;
            } else if (meaning == 49) {
                data[m][n] = life;
                correct = 0;
            } else {
                correct = 1;
                break;
            }
            if (correct == 1) {
                break;
            }
        }
        if (correct == 1) {
            printf("Некорректный ввод\nОживить мир не удалось\n");
            break;
        }
    }
    return correct;
}

void MakeGameSpace(char data[][N + 2]) {
    for (int m = 0; m < M + 2; m++) {
        for (int n = 0; n < N + 2; n++) {
            if (m == 0 || m == M + 1 || n == 0 || n == N + 1) {
                data[m][n] = stringborder;
            } else {data[m][n] = worldPiece;}
        }
    }
}

int MakeLife(char data_1[][N + 2], char data_2[][N + 2]) {
    int counter, flag = 1;
    for (int i = 1; i < M + 1; i++) {
        for (int j = 1; j < N + 1; j++) {
            counter = LifeCounter(data_1, i, j);
            if ((data_1[i][j] == death || data_1[i][j] == worldPiece) && counter == 3) {
                data_2[i][j] = life; flag = 0; continue;
            }
            if (data_1[i][j] == life && (counter == 2 || counter == 3)) {
                data_2[i][j] = life; flag = 0; continue;
            }
            if (data_1[i][j] == life && (counter < 2 || counter > 3)) {
                data_2[i][j] = death; continue;
            }
        }
    }
    return flag;
}

int LifeCounter(char data[][N + 2], int m, int n) {
    int counter = 0;
    for (int i = m - 1, l; i <= m + 1; i++) {
        for (int j = n - 1, k; j <= n + 1; j++) {
            l = i; k = j;
            if (l == m && k == n) {continue;}
            if (l == 0) {l = M;}
            if (l == M + 1) {l = 1;}
            if (k == 0) {k = N;}
            if (k == N + 1) {k = 1;}
            if (data[l][k] == life) {
                counter += 1;
            }
        }
    }
    return counter;
}

void PrintTheWorld(char data[][N + 2]) {
    for (int i = 0; i < M + 2; i++) {
        for (int j = 0; j < N + 2; j++) {
            printf("%c", data[i][j]);
        }
        printf("\n");
    }
}

void CopyDay(char data_1[][N + 2], char data_2[][N + 2]) {
    for (int m = 0; m < M + 2; m++) {
        for (int n = 0; n < N + 2; n++) {
            data_1[m][n] = data_2[m][n];
        }
    }
}

int CompareDays(char data_1[][N + 2], char data_2[][N + 2]) {
    int flag = 1;
    for (int m = 0; m < M + 2; m++) {
        for (int n = 0; n < N + 2; n++) {
            if (data_1[m][n] != data_2[m][n]) {
                flag = 0;
                break;
            }
        }
    }
    return flag;
}

void StopSignals() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void set_keypress(void) {
    struct termios new_settings;
    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    return;
}

void reset_keypress(void) {
    tcsetattr(0, TCSANOW, &stored_settings);
    return;
}
