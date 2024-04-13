static void constructor (LexState *ls, expdesc *t) {
  /* constructor -> '{' [ field { sep field } [sep] ] '}'
     sep -> ',' | ';' */
  FuncState *fs = ls->fs;
  int line = ls->linenumber;
  int pc = luaK_codeABC(fs, OP_NEWTABLE, 0, 0, 0);
  ConsControl cc;
  luaK_code(fs, 0);  /* space for extra arg. */
  cc.na = cc.nh = cc.tostore = 0;
  cc.t = t;
  init_exp(t, VNONRELOC, fs->freereg);  /* table will be at stack top */
  luaK_reserveregs(fs, 1);
  init_exp(&cc.v, VVOID, 0);  /* no value (yet) */
  checknext(ls, '{');
  do {
    lua_assert(cc.v.k == VVOID || cc.tostore > 0);
    if (ls->t.token == '}') break;
    closelistfield(fs, &cc);
    field(ls, &cc);
  } while (testnext(ls, ',') || testnext(ls, ';'));
  check_match(ls, '}', '{', line);
  lastlistfield(fs, &cc);
  luaK_settablesize(fs, pc, t->u.info, cc.na, cc.nh);
}