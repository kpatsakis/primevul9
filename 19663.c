static void statlist (LexState *ls) {
  /* statlist -> { stat [';'] } */
  while (!block_follow(ls, 1)) {
    if (ls->t.token == TK_RETURN) {
      statement(ls);
      return;  /* 'return' must be last statement */
    }
    statement(ls);
  }
}