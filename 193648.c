is_full_opt_exact(OptExact* e)
{
  return (e->len >= OPT_EXACT_MAXLEN ? 1 : 0);
}