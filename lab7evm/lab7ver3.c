#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <cblas.h>
#include <stdlib.h>
#include <math.h>

void CopyMatrix(float *A, float *B, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            A[i * n + j] = B[i * n + j];
}

void PrintMatrix(float *Matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%f ",Matrix[i * n + j]);
        }
        printf("\n");
    }
}

float UnitRate(float *Matrix, int n)
{
    float result = __FLT_MIN__;
    float sum = 0;
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            sum += fabs(Matrix[i * n + j]);
        }
        if (sum > result)
        {
            result = sum;
        }
        sum = 0;
    }
    return result;
}

float InfiniteRate(float *Matrix, int n)
{
    float result = 0;
    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sum += abs(Matrix[i * n + j]);
        }
        if (sum > result)
        {
            result = sum;
        }
        sum = 0;
    }
    return result;
}

float *TranspositionMatrix(float *Matrix, int n)
{
    float *MatrixResult = malloc(sizeof(float) * n * n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            MatrixResult[j * n + i] = Matrix[i * n + j];
            int a = j * n + i;
            int b = i * n + j;
        }
    }
    return MatrixResult;
}

float *DivMatrix(float *Matrix1, float digit, int n)
{
    float *MatrixResult = malloc(sizeof(float) * n * n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            MatrixResult[i * n + j] = Matrix1[i * n + j] / digit;
        }
    }
    return MatrixResult;
}

float *SubMatrix(float *Matrix1, float *Matrix2, int n)
{
    float *MatrixResult = malloc(sizeof(float) * n * n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            MatrixResult[i * n + j] = Matrix1[i * n + j] - Matrix2[i * n + j];
        }
    }
    return MatrixResult;
}

void SumMatrix(float *Matrix1, float *Matrix2,float* MatrixResult, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            MatrixResult[i * n + j] = Matrix1[i * n + j] + Matrix2[i * n + j];
        }
    }
}

void FillMatrix(float *Matrix, int n)
{
    srand(time(0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Matrix[i * n + j] = rand() % 5;
        }
    }
}

void InitializationMatrix(float *Matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Matrix[i * n + j] = 0;
        }
    }
}

void FillIdentityMatrix(float *Matrix, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Matrix[i * n + j] = 0;
            if (i == j)
            {
                Matrix[i * n + j] = 1;
            }
        }
    }
}

void InversionMatrix(float *Matrix, float *MatrixResult, int n, int m)
{
    float *MatrixRFix;
    float *MatrixB;
    float *MatrixIdentity = malloc(sizeof(float) * n * n);
    FillIdentityMatrix(MatrixIdentity, n);

    float digit = UnitRate(Matrix, n) * InfiniteRate(Matrix, n);
    MatrixB = DivMatrix(TranspositionMatrix(Matrix, n), digit, n);


    float *Matrix1 = malloc(sizeof(float) * n * n);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1, MatrixB, n, Matrix, n, 1, Matrix1, n);
    MatrixRFix = SubMatrix(MatrixIdentity, Matrix1, n);

    float *MatrixC = malloc(sizeof(float) * n * n);
    FillIdentityMatrix(MatrixC, n);

    float *MatrixR = malloc(sizeof(float) * n * n);
    CopyMatrix(MatrixRFix, MatrixR, n);

    float *MatrixRCopy = malloc(sizeof(float) * n * n);
    for (int i = 1; i < m; i++)
    {
        SumMatrix(MatrixC, MatrixR, MatrixC, n);
        CopyMatrix(MatrixR, MatrixRCopy, n);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1, MatrixRCopy, n, MatrixRFix, n, 1, MatrixR, n);
    }
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1, MatrixC, n, MatrixB, n, 1, MatrixResult, n);
    free(MatrixRFix);
    free(MatrixB);
    free(MatrixIdentity);
    free(Matrix1);
    free(MatrixC);
    free(MatrixR);
    free(MatrixRCopy);
}

int main()
{
    int n, m;
    printf("Введите размер матрицы: ");
    scanf("%d", &n);
    printf("\n");
    printf("Введите количество членов ряда: ");
    scanf("%d", &m);
    float *Matrix;
    Matrix = malloc(n * n * sizeof(float));
    FillMatrix(Matrix,n);
    float *MatrixResult;
    MatrixResult = malloc(n * n * sizeof(float));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    InversionMatrix(Matrix, MatrixResult,n,m);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Время: %lf секунд.\n",
           end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));

    float *Matrix2 = malloc(n * n * sizeof(float));
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, Matrix, n, MatrixResult, n, 0.0, Matrix2, n);
    printf("%f", UnitRate(Matrix2, n));
    free(MatrixResult);
    free(Matrix);
    free(Matrix2);
    return 0;
}