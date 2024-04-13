distance_multiply(OnigLen d, int m)
{
  if (m == 0) return 0;

  if (d < INFINITE_LEN / m)
    return d * m;
  else
    return INFINITE_LEN;
}