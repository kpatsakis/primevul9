static Proto *addprototype (LexState *ls) {
  Proto *clp;
  lua_State *L = ls->L;
  FuncState *fs = ls->fs;
  Proto *f = fs->f;  /* prototype of current function */
  if (fs->np >= f->sizep) {
    int oldsize = f->sizep;
    luaM_growvector(L, f->p, fs->np, f->sizep, Proto *, MAXARG_Bx, "functions");
    while (oldsize < f->sizep)
      f->p[oldsize++] = NULL;
  }
  f->p[fs->np++] = clp = luaF_newproto(L);
  luaC_objbarrier(L, f, clp);
  return clp;
}