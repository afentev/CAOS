#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n;
    scanf("%d", &n);
    int rows = n;
    int columns = n + 1;

    int rounded = columns;
    if (rounded % 4 != 0) {
        rounded += 4 - rounded % 4;
    }

    double* matrix = aligned_alloc(32, sizeof(double) * rows * rounded);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < n; ++j) {
            scanf("%lf", &matrix[i * rounded + j]);
        }
        for (int j = columns; j < rounded - 1; ++j) {
            matrix[i * rounded + j] = 0;
        }
        scanf("%lf", &matrix[(i + 1) * rounded - 1]);
    }
    for (int attempt = 0; attempt + 1 < rows; ++attempt) {
        int row = attempt;
        double* cur = matrix + row * rounded;
        while (row < rows && cur[row] == 0) {
            ++row;
        }
        if (row == rows) {
            continue;
        }
        for (int next = row + 1; next < rows; ++next) {
            double* target = matrix + next * rounded;
            double fraction = target[row] / cur[row];
            __m256d multiplier = _mm256_broadcast_sd(&fraction);
            int column = row;
            for (; column + 4 <= rounded; column += 4) {
                __m256d upper =
                    _mm256_loadu_pd(matrix + row * rounded + column);
                __m256d lower =
                    _mm256_loadu_pd(matrix + next * rounded + column);
                __m256d product = _mm256_mul_pd(upper, multiplier);
                __m256d subtracted = _mm256_sub_pd(lower, product);
                _mm256_storeu_pd(target, subtracted);
            }
            for (; column < rounded; ++column) {
                matrix[next * rounded + column] -=
                    matrix[row * rounded + column] * fraction;
            }
        }
    }

    for (int row = rows - 1; row >= 0; --row) {
        double lhs = matrix[row * rounded + row];
        double rhs = matrix[(row + 1) * rounded - 1];
        double fraction = rhs / lhs;
        matrix[row * rounded + row] = 1;
        matrix[(row + 1) * rounded - 1] = fraction;

        for (int next = row - 1; next >= 0; --next) {
            double target = matrix[next * rounded + row];
            matrix[next * rounded + row] = 0;
            matrix[(next + 1) * rounded - 1] -= target * fraction;
        }
    }

    for (int i = 0; i < rows; ++i) {
        printf("%lf\n", matrix[(i + 1) * rounded - 1]);
    }
    return 0;
}
