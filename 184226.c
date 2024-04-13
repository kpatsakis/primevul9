int_stack_pop(int_stack* s)
{
  int v;

#ifdef ONIG_DEBUG
  if (s->n <= 0) {
    fprintf(DBGFP, "int_stack_pop: fail empty. %p\n", s);
    return 0;
  }
#endif

  v = s->v[s->n];
  s->n--;
  return v;
}