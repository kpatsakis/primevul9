ops_new(regex_t* reg)
{
  int r;

  if (reg->ops_used >= reg->ops_alloc) {
    r = ops_expand(reg, reg->ops_alloc);
    if (r != ONIG_NORMAL) return r;
  }

  reg->ops_curr = reg->ops + reg->ops_used;
  reg->ops_used++;

  xmemset(reg->ops_curr, 0, sizeof(Operation));
  return ONIG_NORMAL;
}