static void test_then_block (LexState *ls, int *escapelist) {
  /* test_then_block -> [IF | ELSEIF] cond THEN block */
  BlockCnt bl;
  FuncState *fs = ls->fs;
  expdesc v;
  int jf;  /* instruction to skip 'then' code (if condition is false) */
  luaX_next(ls);  /* skip IF or ELSEIF */
  expr(ls, &v);  /* read condition */
  checknext(ls, TK_THEN);
  if (ls->t.token == TK_BREAK) {  /* 'if x then break' ? */
    int line = ls->linenumber;
    luaK_goiffalse(ls->fs, &v);  /* will jump if condition is true */
    luaX_next(ls);  /* skip 'break' */
    enterblock(fs, &bl, 0);  /* must enter block before 'goto' */
    newgotoentry(ls, luaS_newliteral(ls->L, "break"), line, v.t);
    while (testnext(ls, ';')) {}  /* skip semicolons */
    if (block_follow(ls, 0)) {  /* jump is the entire block? */
      leaveblock(fs);
      return;  /* and that is it */
    }
    else  /* must skip over 'then' part if condition is false */
      jf = luaK_jump(fs);
  }
  else {  /* regular case (not a break) */
    luaK_goiftrue(ls->fs, &v);  /* skip over block if condition is false */
    enterblock(fs, &bl, 0);
    jf = v.f;
  }
  statlist(ls);  /* 'then' part */
  leaveblock(fs);
  if (ls->t.token == TK_ELSE ||
      ls->t.token == TK_ELSEIF)  /* followed by 'else'/'elseif'? */
    luaK_concat(fs, escapelist, luaK_jump(fs));  /* must jump over it */
  luaK_patchtohere(fs, jf);
}