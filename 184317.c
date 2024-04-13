mmcl_set_min_max(MinMaxCharLen* l, OnigLen min, OnigLen max, int min_is_sure)
{
  l->min = min;
  l->max = max;
  l->min_is_sure = min_is_sure;
}