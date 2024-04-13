lshift (char *d, int count, int n)
{
char out[64];
for (int i = 0; i < n; i++)
  out[i] = d[(i + count) % n];
for (int i = 0; i < n; i++)
  d[i] = out[i];
}