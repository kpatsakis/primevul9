eqasc2uni (const unsigned char *a, const widechar * b, const int len)
{
  int k;
  for (k = 0; k < len; k++)
    if ((widechar) a[k] != b[k])
      return 0;
  return 1;
}