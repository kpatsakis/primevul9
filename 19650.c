static void suffixedexp (LexState *ls, expdesc *v) {
  /* suffixedexp ->
       primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
  FuncState *fs = ls->fs;
  int line = ls->linenumber;
  primaryexp(ls, v);
  for (;;) {
    switch (ls->t.token) {
      case '.': {  /* fieldsel */
        fieldsel(ls, v);
        break;
      }
      case '[': {  /* '[' exp ']' */
        expdesc key;
        luaK_exp2anyregup(fs, v);
        yindex(ls, &key);
        luaK_indexed(fs, v, &key);
        break;
      }
      case ':': {  /* ':' NAME funcargs */
        expdesc key;
        luaX_next(ls);
        codename(ls, &key);
        luaK_self(fs, v, &key);
        funcargs(ls, v, line);
        break;
      }
      case '(': case TK_STRING: case '{': {  /* funcargs */
        luaK_exp2nextreg(fs, v);
        funcargs(ls, v, line);
        break;
      }
      default: return;
    }
  }
}