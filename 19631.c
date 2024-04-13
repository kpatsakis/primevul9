static void ifstat (LexState *ls, int line) {
  /* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
  FuncState *fs = ls->fs;
  int escapelist = NO_JUMP;  /* exit list for finished parts */
  test_then_block(ls, &escapelist);  /* IF cond THEN block */
  while (ls->t.token == TK_ELSEIF)
    test_then_block(ls, &escapelist);  /* ELSEIF cond THEN block */
  if (testnext(ls, TK_ELSE))
    block(ls);  /* 'else' part */
  check_match(ls, TK_END, TK_IF, line);
  luaK_patchtohere(fs, escapelist);  /* patch escape list to 'if' end */
}