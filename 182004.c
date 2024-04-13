int luaD_pcall (lua_State *L, Pfunc func, void *u,
                ptrdiff_t old_top, ptrdiff_t ef) {
  int status;
  CallInfo *old_ci = L->ci;
  lu_byte old_allowhooks = L->allowhook;
  ptrdiff_t old_errfunc = L->errfunc;
  L->errfunc = ef;
  status = luaD_rawrunprotected(L, func, u);
  if (unlikely(status != LUA_OK)) {  /* an error occurred? */
    StkId oldtop = restorestack(L, old_top);
    L->ci = old_ci;
    L->allowhook = old_allowhooks;
    status = luaF_close(L, oldtop, status);
    oldtop = restorestack(L, old_top);  /* previous call may change stack */
    luaD_seterrorobj(L, status, oldtop);
    luaD_shrinkstack(L);
  }
  L->errfunc = old_errfunc;
  return status;
}