#include <stdio.h>
#include <math.h>
#include "toy.c"
#define VEC_SIZE (TK_K * TK_N)
#define N 4
#define Q 97
#define W 22
#define SQRT_W 33



int mod_exp(int base, int exponent, int modulus) {
    int result = 1;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }

        base = (base * base) % modulus;
        exponent /= 2;
    }

    return result;
}


// Function to reverse bits in an index
int reverse_bits(int index, int bits) {
    int reversed_index = 0;
    for (int i = 0; i < bits; ++i) {
        reversed_index = (reversed_index << 1) | (index & 1);
        index >>= 1;
    }
    return reversed_index;
}

// Function to perform bit-reversal permutation
void permute_bitreverse(short *src, short *dest) {
    // Calculate the number of bits needed to represent the vector size
    int bits = 0;
    int temp_size = VEC_SIZE;
    while (temp_size > 1) {
        temp_size >>= 1;
        bits++;
    }

    // Perform bit-reversal permutation
    for (int i = 0; i < VEC_SIZE; ++i) {
        int reversed_index = reverse_bits(i, bits);
        dest[reversed_index] = src[i];
    }
}

void ntt(int* data, int forward) {
    int x[N];

    // Apply bit-reversal permutation
    permute_bitreverse(data, x);

    int factors[N];
    int factor = 1;

    // Compute factors based on the direction
    if (forward) {
        for (int i = 0; i < N; ++i) {
            factors[i] = mod_exp(SQRT_W, i, Q);
            x[i] = (x[i] * factors[i]) % Q;
        }
    } else {
        for (int i = 0; i < N; ++i) {
            factors[i] = mod_exp(-SQRT_W, i, Q);
            x[i] = (x[i] * factors[i]) % Q;
        }
    }

    for (int s = 1; s <= log2(N); ++s) {
        int m = 1 << s;

        for (int b = 0; b < N; b += m) {
            factor = 1;

            for (int op = 0; op < m / 2; ++op) {
                int a0 = x[b + op];
                int a1 = (x[b + op + m / 2] * factor) % Q;

                x[b + op] = (a0 + a1) % Q;
                x[b + op + m / 2] = (a0 - a1 + Q) % Q;

                factor = (factor * W) % Q;
            }
        }
    }

    // Copy the result back to the data vector
    for (int i = 0; i < N; ++i) {
        data[i] = x[i];
    }
}

int main() {
    int w = 22;
    int q = 97;

    // Calculate w^4 mod q
    int result = 1;
    for (int i = 0; i < 4; ++i) {
        result = (result * w) % q;
    }

    printf("w^4 mod q is: %d\n", result);

    int sqrt_w = 33;
    q = 97;

    // Calculate sqrt(w)^2 mod q
    result = (sqrt_w * sqrt_w) % q;

    printf("sqrt(w)^2 mod q is: %d\n", result);

   // Example usage
    short src[VEC_SIZE];
    short dest[VEC_SIZE];

    // Fill the source vector with some values
    for (int i = 0; i < VEC_SIZE; ++i) {
        src[i] = i + 1;
    }

    // Print the original vector
    printf("Original vector: ");
    for (int i = 0; i < VEC_SIZE; ++i) {
        printf("%d ", src[i]);
    }
    printf("\n");

    // Perform bit-reversal permutation
    permute_bitreverse(src, dest);

    // Print the bit-reversed vector
    printf("Bit-reversed vector: ");
    for (int i = 0; i < VEC_SIZE; ++i) {
        printf("%d ", dest[i]);
    }
    printf("\n");

}