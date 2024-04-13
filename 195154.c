dohash (char *out, char *in, char *key, int forw)
{
int i, j, k;
char pk1[56];
char c[28];
char d[28];
char cd[56];
char ki[16][48];
char pd1[64];
char l[32], r[32];
char rl[64];

permute (pk1, key, perm1, 56);

for (i = 0; i < 28; i++)
  c[i] = pk1[i];
for (i = 0; i < 28; i++)
  d[i] = pk1[i + 28];

for (i = 0; i < 16; i++)
  {
  lshift (c, sc[i], 28);
  lshift (d, sc[i], 28);

  concat (cd, c, d, 28, 28);
  permute (ki[i], cd, perm2, 48);
  }

permute (pd1, in, perm3, 64);

for (j = 0; j < 32; j++)
  {
  l[j] = pd1[j];
  r[j] = pd1[j + 32];
  }

for (i = 0; i < 16; i++)
  {
  char er[48];
  char erk[48];
  char b[8][6];
  char cb[32];
  char pcb[32];
  char r2[32];

  permute (er, r, perm4, 48);

  xor (erk, er, ki[forw ? i : 15 - i], 48);

  for (j = 0; j < 8; j++)
   for (k = 0; k < 6; k++)
     b[j][k] = erk[j * 6 + k];

  for (j = 0; j < 8; j++)
   {
   int m, n;
   m = (b[j][0] << 1) | b[j][5];

   n = (b[j][1] << 3) | (b[j][2] << 2) | (b[j][3] << 1) | b[j][4];

   for (k = 0; k < 4; k++)
     b[j][k] = (sbox[j][m][n] & (1 << (3 - k))) ? 1 : 0;
   }

  for (j = 0; j < 8; j++)
   for (k = 0; k < 4; k++)
     cb[j * 4 + k] = b[j][k];
  permute (pcb, cb, perm5, 32);

  xor (r2, l, pcb, 32);

  for (j = 0; j < 32; j++)
   l[j] = r[j];

  for (j = 0; j < 32; j++)
   r[j] = r2[j];
  }

concat (rl, r, l, 32, 32);

permute (out, rl, perm6, 64);
}