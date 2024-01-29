// toy.h
// toy Post-Quantum Public-Key Cryptosystem
#define TK_K 3
#define TK_N 4

// toy.c
#include <stdio.h>
#include <stdlib.h>

#define TK_Q 97
#define MONT_FACTOR 65280 // beta_q^(lg(n)+1) mod q
#define INVQ 65552        // q^-1 mod 2^16

// polynomial multiplication in Z97[X]/(X^4+1)
static void toy_polmul_naive(short *dst, const short *a, const short *b, int add_to_dst)
{
    if (!add_to_dst)
    {
        for (int i = 0; i < TK_N; ++i)
        {
            dst[i] = 0;
        }
    }

    for (int i = 0; i < TK_N; ++i)
    {
        dst[0] += a[i] * b[(TK_N - i) % TK_N];
        dst[1] += a[i] * b[(TK_N - i + 1) % TK_N];
        dst[2] += a[i] * b[(TK_N - i + 2) % TK_N];
        dst[3] += a[i] * b[(TK_N - i + 3) % TK_N];
    }

    for (int i = 0; i < TK_N; ++i)
    {
        dst[i] %= TK_Q;
        if (dst[i] < 0)
        {
            dst[i] += TK_Q;
        }
    }
}

// Montgomery modular multiplication
#define MONT_MUL(x, y)                                                       \
    do                                                                       \
    {                                                                        \
        x = ((x * y) >> 16) - (((x & 0xFFFF) * INVQ & 0xFFFF) * TK_Q >> 16); \
    } while (0)

// Generate key pair
void toy_gen(short *A, short *t, short *s)
{
    // Fill K*K-matrix A with uniformly random numbers mod q
    for (int i = 0; i < TK_K * TK_K * TK_N; ++i)
    {
        A[i] = rand() % TK_Q;
    }

    // Fill K-vector s & e with small random numbers mod q
    for (int i = 0; i < TK_K * TK_N; ++i)
    {
        int val = rand() & 3;              // Uniform distribution
        s[i] = (val & 1) - (val >> 1 & 1); // Binomial distribution
    }

    // Matrix-vector multiplication in Zq[X]/(X^n+1)
    for (int i = 0; i < TK_K * TK_N; i += TK_N)
    {
        toy_polmul_naive(t + i, A + i, s, 0);
    }
}

// Encrypt
void toy_enc(const short *A, const short *t, int plain, short *u, short *v)
{
    short r[TK_K * TK_N];
    short e1[TK_K * TK_N];
    short e2[TK_N];

    // Fill K-vector r & e1, and the scalar (one-polynomial) e2 with small random numbers mod q
    for (int i = 0; i < TK_K * TK_N; ++i)
    {
        r[i] = rand() % TK_Q;
        e1[i] = rand() % TK_Q;
    }

    for (int i = 0; i < TK_N; ++i)
    {
        e2[i] = rand() % TK_Q;
    }

    // Matrix transpose-vector multiplication
    for (int i = 0; i < TK_K; ++i)
    {
        for (int j = 0; j < TK_N; ++j)
        {
            u[i] += A[i * TK_N + j] * r[j];
        }
    }

    // Vector-vector multiplication
    toy_polmul_naive(v, t, r, 0);

    // Add e1
    for (int i = 0; i < TK_K * TK_N; ++i)
    {
        v[i] += e1[i];
        v[i] %= TK_Q;
        if (v[i] < 0)
        {
            v[i] += TK_Q;
        }
    }

    // Add e2 and message bits
    for (int i = 0; i < TK_N; ++i)
    {
        v[i] += e2[i] + ((plain >> i) & 1) * TK_Q / 2;
        v[i] %= TK_Q;
        if (v[i] < 0)
        {
            v[i] += TK_Q;
        }
    }
}

// Decrypt
int toy_dec(const short *s, const short *u, const short *v)
{
    short p[TK_N];
    int plain = 0;

    // Vector-vector multiplication
    toy_polmul_naive(p, s, u, 0);

    // Subtract p from v
    for (int i = 0; i < TK_N; ++i)
    {
        v[i] -= p[i];
        v[i] %= TK_Q;
        if (v[i] < 0)
        {
            v[i] += TK_Q;
        }
    }

    // Extract message bits
    for (int i = 0; i < TK_N; ++i)
    {
        int val = v[i];
        if (val > TK_Q / 2)
        {
            val -= TK_Q;
        }
        int bit = abs(val) > TK_Q / 4;
        plain |= bit << i;
    }

    return plain;
}

int main()
{
    short A[TK_K * TK_K * TK_N];
    short t[TK_K * TK_N];
    short s[TK_K * TK_N];
    short u[TK_K * TK_N] = {0};
    short v[TK_K * TK_N] = {0};

    // Generate keys
    toy_gen(A, t, s);

    // Encrypt
    int plain = 7; // Replace with your desired plaintext
    toy_enc(A, t, plain, u, v);

    // Decrypt
    int decrypted_plain = toy_dec(s, u, v);

    // Verify correctness
    printf("Original Plain: %d\n", plain);
    printf("Decrypted Plain: %d\n", decrypted_plain);

    return 0;
}
