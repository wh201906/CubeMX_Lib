#include <math.h>
#include <stdio.h>

#define L 32
#define PI 3.14159265358979f // in arm_math.h

float a0 = 0.21557895f;
float a1 = 0.41663158f;
float a2 = 0.277263158f;
float a3 = 0.083578947f;
float a4 = 0.006947368f;

float hannWindow[L], flatWindow[L]; // float->float32_t
int i;

// the same as matlab, test passed
void hannTest()
{
    hannWindow[0] = 0;
    // hann(L, 'periodic')
    for (i = 1; i < L / 2 + 1; i++)
    {
        hannWindow[i] = 0.5 * (1 - cosf(2 * PI * i / L));
        hannWindow[L - i] = hannWindow[i];
    }
    printf("hann:\n");
    for (i = 0; i < L; i++)
    {
        printf("%f, ", hannWindow[i]);
    }
    printf("\n");
}

void flatTest()
{
    flatWindow[0] = a0 - a1 + a2 - a3 + a4;
    // flattopwin(L, 'periodic')
    for (i = 1; i < L / 2 + 1; i++)
    {
        flatWindow[i] = a0;
        flatWindow[i] -= a1 * cosf(2 * PI * i / L);
        flatWindow[i] += a2 * cosf(4 * PI * i / L);
        flatWindow[i] -= a3 * cosf(6 * PI * i / L);
        flatWindow[i] += a4 * cosf(8 * PI * i / L);
        flatWindow[L - i] = flatWindow[i];
    }
    printf("flat:\n");
    for (i = 0; i < L; i++)
    {
        printf("%f, ", flatWindow[i]);
    }
    printf("\n");
}

int main()
{
    hannTest();
    flatTest();
    return 0;
}