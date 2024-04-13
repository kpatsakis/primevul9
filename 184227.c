mmcl_multiply(MinMaxCharLen* to, int m)
{
  to->min = distance_multiply(to->min, m);
  to->max = distance_multiply(to->max, m);
}