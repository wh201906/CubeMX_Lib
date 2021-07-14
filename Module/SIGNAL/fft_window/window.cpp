#include <math.h>
#include <stdio.h>

#define L 32
#define PI 3.14159265358979f // in arm_math.h

float a0, a1, a2, a3, a4;

float coef[L]; // float->float32_t
int i;

void out()
{
    for (i = 0; i < L; i++)
    {
        printf("%f, ", coef[i]);
    }
    printf("\n");
}

// the same as matlab, test passed
void hannTest()
{
    coef[0] = 0;
    // hann(L, 'periodic')
    for (i = 1; i < L / 2 + 1; i++)
    {
        coef[i] = 0.5 * (1 - cosf(2 * PI * i / L));
        coef[L - i] = coef[i];
    }
    printf("hann:\n");
}

void flatTest()
{
    a0 = 0.21557895f;
    a1 = 0.41663158f;
    a2 = 0.277263158f;
    a3 = 0.083578947f;
    a4 = 0.006947368f;
    coef[0] = a0 - a1 + a2 - a3 + a4;
    // flattopwin(L, 'periodic')
    for (i = 1; i < L / 2 + 1; i++)
    {
        coef[i] = a0;
        coef[i] -= a1 * cosf(2 * PI * i / L);
        coef[i] += a2 * cosf(4 * PI * i / L);
        coef[i] -= a3 * cosf(6 * PI * i / L);
        coef[i] += a4 * cosf(8 * PI * i / L);
        coef[L - i] = coef[i];
    }
    printf("flat:\n");
}

void hammingTest()
{
    // hamming(L, 'periodic')
    coef[0] = 0.08;
    for (i = 1; i < L / 2 + 1; i++)
    {
        coef[i] = 0.54 - 0.46 * cosf(2 * PI * i / L);
        coef[L - i] = coef[i];
    }
    printf("hamming:\n");
}

void blackmanTest()
{
    // blackman(L, 'periodic')
    a0 = 0.42;
    a1 = 0.5;
    a2 = 0.08;
    coef[0] = a0 - a1 + a2;
    for (i = 1; i < L / 2 + 1; i++)
    {
        coef[i] = a0;
        coef[i] -= a1 * cosf(2 * PI * i / L);
        coef[i] += a2 * cosf(4 * PI * i / L);
        coef[L - i] = coef[i];
    }
    printf("blackman:\n");
}

void triangTest()
{
    // the L is always even
    // triangWindowP = triang(L)
    for (i = 0; i < L / 2; i++)
    {
        coef[i] = (2.0 * i + 1) / L;
        coef[L - 1 - i] = coef[i];
    }
    printf("Triang:\n");
}

int main()
{
    hannTest(); // test passed
    out();
    flatTest(); // test passed
    out();
    hammingTest(); // test passed
    out();
    blackmanTest(); // test passed
    out();
    triangTest(); // test passed
    out();
    return 0;
}