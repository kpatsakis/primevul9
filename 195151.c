mdfour (uschar *out, uschar *in, int n)
{
uschar buf[128];
uint32x M[16];
uint32x b = n * 8;
int i;

A = 0x67452301;
B = 0xefcdab89;
C = 0x98badcfe;
D = 0x10325476;

while (n > 64)
  {
  copy64 (M, in);
  spa_mdfour64 (M);
  in += 64;
  n -= 64;
  }

for (i = 0; i < 128; i++)
  buf[i] = 0;
memcpy (buf, in, n);
buf[n] = 0x80;

if (n <= 55)
  {
  copy4 (buf + 56, b);
  copy64 (M, buf);
  spa_mdfour64 (M);
  }
else
  {
  copy4 (buf + 120, b);
  copy64 (M, buf);
  spa_mdfour64 (M);
  copy64 (M, buf + 64);
  spa_mdfour64 (M);
  }

for (i = 0; i < 128; i++)
  buf[i] = 0;
copy64 (M, buf);

copy4 (out, A);
copy4 (out + 4, B);
copy4 (out + 8, C);
copy4 (out + 12, D);

A = B = C = D = 0;
}