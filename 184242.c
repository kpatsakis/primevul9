mmcl_alt_merge(MinMaxCharLen* to, MinMaxCharLen* alt)
{
  if (to->min > alt->min) {
    to->min         = alt->min;
    to->min_is_sure = alt->min_is_sure;
  }
  else if (to->min == alt->min) {
    if (alt->min_is_sure != FALSE)
      to->min_is_sure = TRUE;
  }

  if (to->max < alt->max) to->max = alt->max;
}