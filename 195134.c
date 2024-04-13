spa_mdfour64 (uint32x * M)
{
int j;
uint32x AA, BB, CC, DD;
uint32x X[16];

for (j = 0; j < 16; j++)
  X[j] = M[j];

AA = A;
BB = B;
CC = C;
DD = D;

ROUND1 (A, B, C, D, 0, 3);
ROUND1 (D, A, B, C, 1, 7);
ROUND1 (C, D, A, B, 2, 11);
ROUND1 (B, C, D, A, 3, 19);
ROUND1 (A, B, C, D, 4, 3);
ROUND1 (D, A, B, C, 5, 7);
ROUND1 (C, D, A, B, 6, 11);
ROUND1 (B, C, D, A, 7, 19);
ROUND1 (A, B, C, D, 8, 3);
ROUND1 (D, A, B, C, 9, 7);
ROUND1 (C, D, A, B, 10, 11);
ROUND1 (B, C, D, A, 11, 19);
ROUND1 (A, B, C, D, 12, 3);
ROUND1 (D, A, B, C, 13, 7);
ROUND1 (C, D, A, B, 14, 11);
ROUND1 (B, C, D, A, 15, 19);

ROUND2 (A, B, C, D, 0, 3);
ROUND2 (D, A, B, C, 4, 5);
ROUND2 (C, D, A, B, 8, 9);
ROUND2 (B, C, D, A, 12, 13);
ROUND2 (A, B, C, D, 1, 3);
ROUND2 (D, A, B, C, 5, 5);
ROUND2 (C, D, A, B, 9, 9);
ROUND2 (B, C, D, A, 13, 13);
ROUND2 (A, B, C, D, 2, 3);
ROUND2 (D, A, B, C, 6, 5);
ROUND2 (C, D, A, B, 10, 9);
ROUND2 (B, C, D, A, 14, 13);
ROUND2 (A, B, C, D, 3, 3);
ROUND2 (D, A, B, C, 7, 5);
ROUND2 (C, D, A, B, 11, 9);
ROUND2 (B, C, D, A, 15, 13);

ROUND3 (A, B, C, D, 0, 3);
ROUND3 (D, A, B, C, 8, 9);
ROUND3 (C, D, A, B, 4, 11);
ROUND3 (B, C, D, A, 12, 15);
ROUND3 (A, B, C, D, 2, 3);
ROUND3 (D, A, B, C, 10, 9);
ROUND3 (C, D, A, B, 6, 11);
ROUND3 (B, C, D, A, 14, 15);
ROUND3 (A, B, C, D, 1, 3);
ROUND3 (D, A, B, C, 9, 9);
ROUND3 (C, D, A, B, 5, 11);
ROUND3 (B, C, D, A, 13, 15);
ROUND3 (A, B, C, D, 3, 3);
ROUND3 (D, A, B, C, 11, 9);
ROUND3 (C, D, A, B, 7, 11);
ROUND3 (B, C, D, A, 15, 15);

A += AA;
B += BB;
C += CC;
D += DD;

A &= 0xFFFFFFFF;
B &= 0xFFFFFFFF;
C &= 0xFFFFFFFF;
D &= 0xFFFFFFFF;

for (j = 0; j < 16; j++)
  X[j] = 0;
}