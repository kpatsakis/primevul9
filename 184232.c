clear_opt_exact(OptStr* e)
{
  mml_clear(&e->mm);
  clear_opt_anc_info(&e->anc);
  e->reach_end = 0;
  e->len       = 0;
  e->s[0]      = '\0';
}