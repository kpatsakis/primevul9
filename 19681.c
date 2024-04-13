static void body (LexState *ls, expdesc *e, int ismethod, int line) {
  /* body ->  '(' parlist ')' block END */
  FuncState new_fs;
  BlockCnt bl;
  new_fs.f = addprototype(ls);
  new_fs.f->linedefined = line;
  open_func(ls, &new_fs, &bl);
  checknext(ls, '(');
  if (ismethod) {
    new_localvarliteral(ls, "self");  /* create 'self' parameter */
    adjustlocalvars(ls, 1);
  }
  parlist(ls);
  checknext(ls, ')');
  statlist(ls);
  new_fs.f->lastlinedefined = ls->linenumber;
  check_match(ls, TK_END, TK_FUNCTION, line);
  codeclosure(ls, e);
  close_func(ls);
}