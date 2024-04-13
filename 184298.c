mmcl_repeat_range_multiply(MinMaxCharLen* to, int mlow, int mhigh)
{
  to->min = distance_multiply(to->min, mlow);

  if (IS_INFINITE_REPEAT(mhigh))
    to->max = INFINITE_LEN;
  else
    to->max = distance_multiply(to->max, mhigh);
}