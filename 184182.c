mml_add(MinMaxLen* to, MinMaxLen* add)
{
  to->min = distance_add(to->min, add->min);
  to->max = distance_add(to->max, add->max);
}