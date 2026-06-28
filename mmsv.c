// Multithreaded Magic Square Validator
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

int n;
int **matrix;
int magic_constant;
int *row_results;
int *col_results;
int diag_main_result;
int diag_secondary_result;
int unique_result;
int score;
pthread_mutex_t score_mutex;

typedef struct {
    int thread_id;
} ThreadArgs;

// Row validation thread
void *validate_rows(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    for (int i = 0; i < n; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += matrix[i][j];
        }

        sleep(1);

        pthread_mutex_lock(&score_mutex);
        if (sum == magic_constant) {
            row_results[i] = 1;
            score++;
        } else {
            row_results[i] = 0;
            score--;
        }
        pthread_mutex_unlock(&score_mutex);
    }

    printf("Thread ID-%d: Row checks completed.\n", args->thread_id);
    pthread_exit(NULL);
}

// Column validation thread
void *validate_columns(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    for (int j = 0; j < n; j++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += matrix[i][j];
        }

        sleep(1);

        pthread_mutex_lock(&score_mutex);
        if (sum == magic_constant) {
            col_results[j] = 1;
            score++;
        } else {
            col_results[j] = 0;
            score--;
        }
        pthread_mutex_unlock(&score_mutex);
    }

    printf("Thread ID-%d: Column checks completed.\n", args->thread_id);
    pthread_exit(NULL);
}

// Diagonal validation thread
void *validate_diagonals(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    int main_sum = 0;
    int sec_sum = 0;
    for (int i = 0; i < n; i++) {
        main_sum += matrix[i][i];
        sec_sum += matrix[i][n - 1 - i];
    }

    sleep(1);

    pthread_mutex_lock(&score_mutex);
    if (main_sum == magic_constant) {
        diag_main_result = 1;
        score++;
    } else {
        diag_main_result = 0;
        score--;
    }
    if (sec_sum == magic_constant) {
        diag_secondary_result = 1;
        score++;
    } else {
        diag_secondary_result = 0;
        score--;
    }
    pthread_mutex_unlock(&score_mutex);

    printf("Thread ID-%d: Diagonal checks completed.\n", args->thread_id);
    pthread_exit(NULL);
}

// Uniqueness validation thread
void *validate_uniqueness(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    int *count = (int *)calloc(n * n + 1, sizeof(int));
    if (!count) {
        pthread_exit(NULL);
    }

    bool is_unique = true;
    for (int i = 0; i < n && is_unique; i++) {
        for (int j = 0; j < n; j++) {
            int val = matrix[i][j];
            if (val < 1 || val > n * n) {
                is_unique = false;
                break;
            }
            count[val]++;
            if (count[val] > 1) {
                is_unique = false;
                break;
            }
        }
    }

    free(count);

    sleep(1);

    pthread_mutex_lock(&score_mutex);
    if (is_unique) {
        unique_result = 1;
        score++;
    } else {
        unique_result = 0;
        score--;
    }
    pthread_mutex_unlock(&score_mutex);

    printf("Thread ID-%d: Uniqueness check completed.\n", args->thread_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    if (fscanf(fp, "%d", &n) != 1 || n < 1) {
        fprintf(stderr, "Error: Invalid matrix size\n");
        fclose(fp);
        return 1;
    }

    matrix = (int **)malloc(n * sizeof(int *));
    if (!matrix) {
        fclose(fp);
        return 1;
    }
    for (int i = 0; i < n; i++) {
        matrix[i] = (int *)malloc(n * sizeof(int));
        if (!matrix[i]) {
            fclose(fp);
            return 1;
        }
        for (int j = 0; j < n; j++) {
            if (fscanf(fp, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Error: Invalid matrix data\n");
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);

    magic_constant = n * (n * n + 1) / 2;

    row_results = (int *)calloc(n, sizeof(int));
    col_results = (int *)calloc(n, sizeof(int));
    diag_main_result = 0;
    diag_secondary_result = 0;
    unique_result = 0;
    score = 0;

    if (!row_results || !col_results) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    pthread_mutex_init(&score_mutex, NULL);

    pthread_t threads[4];
    ThreadArgs args[4];

    for (int i = 0; i < 4; i++) {
        args[i].thread_id = i + 1;
    }

    pthread_create(&threads[0], NULL, validate_rows, &args[0]);
    pthread_create(&threads[1], NULL, validate_columns, &args[1]);
    pthread_create(&threads[2], NULL, validate_diagonals, &args[2]);
    pthread_create(&threads[3], NULL, validate_uniqueness, &args[3]);

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Magic Square Report ---\n");

    bool all_rows_valid = true;
    for (int i = 0; i < n; i++) {
        if (!row_results[i]) {
            all_rows_valid = false;
            break;
        }
    }
    if (all_rows_valid) {
        printf("Rows: All Valid\n");
    } else {
        printf("Rows:\n");
        for (int i = 0; i < n; i++) {
            if (!row_results[i]) {
                printf("Row %d Invalid\n", i + 1);
            }
        }
    }

    bool all_cols_valid = true;
    for (int j = 0; j < n; j++) {
        if (!col_results[j]) {
            all_cols_valid = false;
            break;
        }
    }
    if (all_cols_valid) {
        printf("Cols: All Valid\n");
    } else {
        printf("Cols:\n");
        for (int j = 0; j < n; j++) {
            if (!col_results[j]) {
                printf("Col %d Invalid\n", j + 1);
            }
        }
    }

    if (diag_main_result && diag_secondary_result) {
        printf("Diags: All Valid\n");
    } else {
        printf("Diags:");
        if (!diag_main_result) {
            printf(" Main Diag Invalid");
        }
        if (!diag_secondary_result) {
            printf(" Secondary Diag Invalid");
        }
        printf("\n");
    }

    if (unique_result) {
        printf("Unique: Passed\n");
    } else {
        printf("Unique: Failed (Duplicates found)\n");
    }

    int max_score = 2 * n + 3;
    printf("Final Score: %d / %d\n", score, max_score);

    if (score == max_score) {
        printf("RESULT: VALID MAGIC SQUARE\n");
    } else {
        printf("RESULT: INVALID MAGIC SQUARE\n");
    }

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(row_results);
    free(col_results);
    pthread_mutex_destroy(&score_mutex);

    return 0;
}
