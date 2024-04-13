static void init_exp (expdesc *e, expkind k, int i) {
  e->f = e->t = NO_JUMP;
  e->k = k;
  e->u.info = i;
}