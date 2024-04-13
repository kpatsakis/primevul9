mmcl_set(MinMaxCharLen* l, OnigLen len)
{
  l->min = len;
  l->max = len;
  l->min_is_sure = TRUE;
}