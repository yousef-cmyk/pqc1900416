#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "toy.c"

int main()
{
    short matrix[K_SIZE * K_SIZE * N_SIZE];
    short term[K_SIZE * N_SIZE];
    short noise[K_SIZE * N_SIZE];
    generate_polys(matrix, term, noise);

    short u[K_SIZE * N_SIZE];
    short v[N_SIZE];
    for (int i = 1; i <= 16; i++)
    {
        encrypt(matrix, term, i, u, v);
        int plaintext;
        plaintext = decrypt(noise, u, v);

        printf("%d\n", plaintext);
    }
}
