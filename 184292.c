make_int_stack(int_stack** rs, int init_size)
{
  int_stack* s;
  int* v;

  *rs = 0;

  s = xmalloc(sizeof(*s));
  if (IS_NULL(s)) return ONIGERR_MEMORY;

  v = (int* )xmalloc(sizeof(int) * init_size);
  if (IS_NULL(v)) {
    xfree(s);
    return ONIGERR_MEMORY;
  }

  s->n = 0;
  s->alloc = init_size;
  s->v = v;

  *rs = s;
  return ONIG_NORMAL;
}