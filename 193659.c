is_equal_mml(MinMax* a, MinMax* b)
{
  return (a->min == b->min && a->max == b->max) ? 1 : 0;
}