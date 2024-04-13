mml_is_equal(MinMaxLen* a, MinMaxLen* b)
{
  return a->min == b->min && a->max == b->max;
}