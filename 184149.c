len_multiply_cmp(OnigLen x, int y, OnigLen v)
{
  if (x == 0 || y == 0) return -1;

  if (x < INFINITE_LEN / y) {
    OnigLen xy = x * (OnigLen )y;
    if (xy > v) return 1;
    else {
      if (xy == v) return 0;
      else return -1;
    }
  }
  else
    return v == INFINITE_LEN ? 0 : 1;
}