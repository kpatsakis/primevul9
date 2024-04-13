mmcl_add(MinMaxCharLen* to, MinMaxCharLen* add)
{
  to->min = distance_add(to->min, add->min);
  to->max = distance_add(to->max, add->max);

  to->min_is_sure = add->min_is_sure != FALSE && to->min_is_sure != FALSE;
}