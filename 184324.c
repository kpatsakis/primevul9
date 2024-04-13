mml_alt_merge(MinMaxLen* to, MinMaxLen* alt)
{
  if (to->min > alt->min) to->min = alt->min;
  if (to->max < alt->max) to->max = alt->max;
}