permute (char *out, char *in, uschar * p, int n)
{
for (int i = 0; i < n; i++)
  out[i] = in[p[i] - 1];
}