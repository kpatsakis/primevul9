static void listfield (LexState *ls, ConsControl *cc) {
  /* listfield -> exp */
  expr(ls, &cc->v);
  cc->tostore++;
}