int_stack_push(int_stack* s, int v)
{
  if (s->n >= s->alloc) {
    int new_size = s->alloc * 2;
    int* nv = (int* )xrealloc(s->v, sizeof(int) * new_size);
    if (IS_NULL(nv)) return ONIGERR_MEMORY;

    s->alloc = new_size;
    s->v = nv;
  }

  s->v[s->n] = v;
  s->n++;
  return ONIG_NORMAL;
}