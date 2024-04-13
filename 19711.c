static void funcstat (LexState *ls, int line) {
  /* funcstat -> FUNCTION funcname body */
  int ismethod;
  expdesc v, b;
  luaX_next(ls);  /* skip FUNCTION */
  ismethod = funcname(ls, &v);
  body(ls, &b, ismethod, line);
  check_readonly(ls, &v);
  luaK_storevar(ls->fs, &v, &b);
  luaK_fixline(ls->fs, line);  /* definition "happens" in the first line */
}