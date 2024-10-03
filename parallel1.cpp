#include <pthread.h>
#include <stdio.h>
#include <time.h>

const int nMax = 8000;
double matrixA[nMax][nMax];
double vectorB[nMax];
double resultC[nMax];
const int nT = 4; // Number of threads

                  // Функция для параллельного умножения
                  void* multiplication(void* arg) {
                      int* param = (int*)arg;
                      int start = param[0];
                      int finish = param[1];
                  
                      double result = 0;
                      for (int k = start; k < finish; k++) {
                          result = 0;
                          for (int i = 0; i < param[2]; i++) {  // param[2] - текущая размерность n
                              result += matrixA[k][i] * vectorB[i];
                          }
                          resultC[k] = result;
                      }
                  
                      return NULL;
                  }

int main(int argc, char* argv[]) {
    FILE* file;
    int dimensions[] = {500, 1000, 2000, 4000, 8000};  // Размерности для тестирования
    int num_tests = sizeof(dimensions) / sizeof(dimensions[0]);  // Количество тестов

    for (int test = 0; test < num_tests; test++) {
        int n = dimensions[test];

        // Инициализация матрицы и вектора
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrixA[i][j] = i + j;  // Пример заполнения матрицы
            }
        }

        for (int i = 0; i < n; i++) {
            vectorB[i] = 10 * i;  // Пример заполнения вектора
        }

        // Инициализация результата
        for (int i = 0; i < n; i++) {
            resultC[i] = 0;
        }

        // Параметры потоков
        pthread_t thread[nT];
        int threadParam[nT][3];  // Дополнительный параметр для передачи размерности n
        for (int j = 0; j < nT; j++) {
            threadParam[j][0] = (n / nT) * j;
            threadParam[j][1] = (n / nT) * (j + 1);
            threadParam[j][2] = n;  // Передаем текущую размерность
        }
        threadParam[nT - 1][1] = n;  // Последний поток выполняет до конца

        // Измерение времени выполнения
        int t0 = clock();

        // Создание потоков
        for (int j = 0; j < nT; j++) {
            pthread_create(&thread[j], NULL, &multiplication, &threadParam[j]);
        }

        // Ожидание завершения потоков
        for (int i = 0; i < nT; i++) {
            pthread_join(thread[i], NULL);
        }

        int t1 = clock();
        int dt = t1 - t0;

        // Запись результатов в файл
        file = fopen("finC.txt", "a");
        if (file == NULL) {
            printf("Не удалось открыть файл для записи.\n");
            return 1;
        }
        fprintf(file, "\nTime of multiplication matrix[%d][%d] by vector[%d] = %d microseconds\n", n, n, n, dt);
        for (int i = 0; i < n; i++) {
            fprintf(file, "%.2lf ", resultC[i]);
        }
        fprintf(file, "\n");  // Переход на новую строку после каждой размерности
        fclose(file);

        printf("Тест с размерностью %d завершен, результаты записаны в файл.\n", n);
    }

    return 0;
}
