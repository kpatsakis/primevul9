positive_int_multiply(int x, int y)
{
  if (x == 0 || y == 0) return 0;

  if (x < INT_MAX / y)
    return x * y;
  else
    return -1;
}