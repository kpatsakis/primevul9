mmcl_fixed(MinMaxCharLen* c)
{
  return (c->min == c->max && c->min != INFINITE_LEN);
}