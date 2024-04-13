static void checknext (LexState *ls, int c) {
  check(ls, c);
  luaX_next(ls);
}