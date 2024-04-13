bitset_is_empty(BitSetRef bs)
{
  int i;

  for (i = 0; i < (int )BITSET_REAL_SIZE; i++) {
    if (bs[i] != 0) return 0;
  }
  return 1;
}