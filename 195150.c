copy64 (uint32x * M, uschar *in)
{
int i;

for (i = 0; i < 16; i++)
  M[i] = (in[i * 4 + 3] << 24) | (in[i * 4 + 2] << 16) |
    (in[i * 4 + 1] << 8) | (in[i * 4 + 0] << 0);
}