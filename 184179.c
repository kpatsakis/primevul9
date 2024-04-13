free_int_stack(int_stack* s)
{
  if (IS_NOT_NULL(s)) {
    if (IS_NOT_NULL(s->v))
      xfree(s->v);
    xfree(s);
  }
}