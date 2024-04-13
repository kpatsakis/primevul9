void luaD_hookcall (lua_State *L, CallInfo *ci) {
  int hook = (ci->callstatus & CIST_TAIL) ? LUA_HOOKTAILCALL : LUA_HOOKCALL;
  Proto *p;
  if (!(L->hookmask & LUA_MASKCALL))  /* some other hook? */
    return;  /* don't call hook */
  p = clLvalue(s2v(ci->func))->p;
  L->top = ci->top;  /* prepare top */
  ci->u.l.savedpc++;  /* hooks assume 'pc' is already incremented */
  luaD_hook(L, hook, -1, 1, p->numparams);
  ci->u.l.savedpc--;  /* correct 'pc' */
}