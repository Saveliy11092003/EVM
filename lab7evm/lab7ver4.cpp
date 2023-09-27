#include <iostream>
#include <limits.h>
#include <immintrin.h>
#include <avxintrin.h>
#include <cblas.h>

using namespace std;

void CopyMatrix(float *Matrix, float *MatrixC, int n)
{
    __m128 a;
    for (int i = 0; i < n * n; i += 4)
    {
        a = _mm_load_ps(Matrix + i);
        _mm_store_ps(MatrixC + i, a);
    }
}

void PrintMatrix(float *Matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%.4f ", Matrix[i * n + j]);
        }
        printf("\n");
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

float *SumMatrix(float *Matrix1, float *Matrix2, int n)
{
    float *MatrixResult = new float[n * n];
    __m128 a;
    __m128 b;
    __m128 c;
    for (int i = 0; i < n * n; i += 4)
    {
        a = _mm_load_ps(Matrix1 + i);
        b = _mm_load_ps(Matrix2 + i);
        c = _mm_add_ps(a, b);
        _mm_store_ps(MatrixResult + i, c);
    }
    return MatrixResult;
}

void SubMatrix(float *Matrix1, float *Matrix2, float *MatrixResult, int n)
{
    __m128 a;
    __m128 b;
    __m128 c;
    for (int i = 0; i < n * n; i += 4)
    {
        a = _mm_load_ps(Matrix1 + i);
        b = _mm_load_ps(Matrix2 + i);
        c = _mm_sub_ps(a, b);
        _mm_store_ps(MatrixResult + i, c);
    }
}

void DivMatrix(float *Matrix1, float *MatrixResult, float digit, int n)
{
    __m128 a;
    __m128 b;
    __m128 c;
    b = _mm_set1_ps(digit);
    for (int i = 0; i < n * n; i += 4)
    {
        a = _mm_load_ps(Matrix1 + i);
        c = _mm_div_ps(a, b);
        _mm_store_ps(MatrixResult + i, c);
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
    InitializationMatrix(Matrix, n);
    for (int i = 0; i < n; i++)
    {
        Matrix[i * n + i] = 1;
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

void TranspositionMatrix(float *Matrix, float *MatrixResult, int n)
{

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            MatrixResult[j * n + i] = Matrix[i * n + j];
            int a = j * n + i;
            int b = i * n + j;
        }
    }
}

void MultMatrix(const float *A, const float *B, float *C, int N)
{
    for (int i = 0; i < N; ++i)
    {
        float *c = C + i * N;
        for (int j = 0; j < N; j += 4)
        {
            _mm_store_ps(c + j + 0, _mm_setzero_ps());
        }
        for (int k = 0; k < N; ++k)
        {
            const float *b = B + k * N;
            __m128 a = _mm_set1_ps(A[i * N + k]);
            for (int j = 0; j < N; j += 4)
            {
                _mm_store_ps(c + j, _mm_fmadd_ps(a, _mm_load_ps(b + j), _mm_load_ps(c + j)));
            }
        }
    }
}

void InversionMatrix(float *Matrix, float *MatrixResult, int n, int m)
{
    float *MatrixRFix;
    MatrixRFix = (float *)_mm_malloc(n * n * sizeof(float), 16);
    float *MatrixB;
    MatrixB = (float *)_mm_malloc(n * n * sizeof(float), 16);
    float *MatrixIdentity;
    MatrixIdentity = (float *)_mm_malloc(n * n * sizeof(float), 16);
    FillIdentityMatrix(MatrixIdentity, n);
    float digit = UnitRate(Matrix, n) * InfiniteRate(Matrix, n);
    float *MatrixTransposition;
    MatrixTransposition = (float *)_mm_malloc(n * n * sizeof(float), 16);
    TranspositionMatrix(Matrix, MatrixTransposition, n);
    DivMatrix(MatrixTransposition, MatrixB, digit, n);
    float *Matrix1;
    Matrix1 = (float *)_mm_malloc(n * n * sizeof(float), 16);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, MatrixB, n, Matrix, n, 0.0, Matrix1, n);
    SubMatrix(MatrixIdentity, Matrix1, MatrixRFix, n);
    float *MatrixC;
    MatrixC = (float *)_mm_malloc(n * n * sizeof(float), 16);
    FillIdentityMatrix(MatrixC, n);
    float *MatrixR;
    MatrixR = (float *)_mm_malloc(n * n * sizeof(float), 16);
    CopyMatrix(MatrixRFix, MatrixR, n);
    for (int i = 1; i < m; i++)
    {
        MatrixC = SumMatrix(MatrixC, MatrixR, n);
        float *MatrixRCopy;
        MatrixRCopy = (float *)_mm_malloc(n * n * sizeof(float), 16);
        CopyMatrix(MatrixR, MatrixRCopy, n);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, MatrixRCopy, n, MatrixRFix, n, 0.0, MatrixR, n);
    }
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, MatrixC, n, MatrixB, n, 0.0, MatrixResult, n);
    _mm_free(MatrixRFix);
    _mm_free(MatrixB);
    _mm_free(MatrixIdentity);
    _mm_free(Matrix1);
    _mm_free(MatrixC);
    _mm_free(MatrixR);
    _mm_free(MatrixTransposition);
}

int main()
{
    int n, m;
    cout << "Введите размер матрицы: ";
    cin >> n;
    cout << "\n"
         << "Введите количество членов ряда: ";
    cin >> m;
    float *Matrix1;
    Matrix1 = (float *)_mm_malloc(n * n * sizeof(float), 16);
    float *MatrixResult;
    MatrixResult = (float *)_mm_malloc(n * n * sizeof(float), 16);
    float *MatrixCheck = new float[n * n];
    FillMatrix(Matrix1, n);
    // PrintMatrix(Matrix1, n);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    InversionMatrix(Matrix1, MatrixResult, n, m);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Время: %lf секунд.\n",
           end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));
    // PrintMatrix(MatrixResult, n);
    MultMatrix(Matrix1, MatrixResult, MatrixCheck, n);
    cout << "Первая норма: " << UnitRate(MatrixCheck, n);
    _mm_free(MatrixResult);
    _mm_free(Matrix1);
    _mm_free(MatrixCheck);
    return 0;
}