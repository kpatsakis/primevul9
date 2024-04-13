find_all_exitpolicy(smartlist_t *s)
{
  smartlist_t *out = smartlist_create();
  SMARTLIST_FOREACH(s, directory_token_t *, t,
      if (t->tp == K_ACCEPT || t->tp == K_ACCEPT6 ||
          t->tp == K_REJECT || t->tp == K_REJECT6)
        smartlist_add(out,t));
  return out;
}