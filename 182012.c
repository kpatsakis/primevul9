void luaD_tryfuncTM (lua_State *L, StkId func) {
  const TValue *tm = luaT_gettmbyobj(L, s2v(func), TM_CALL);
  StkId p;
  if (unlikely(ttisnil(tm)))
    luaG_typeerror(L, s2v(func), "call");  /* nothing to call */
  for (p = L->top; p > func; p--)  /* open space for metamethod */
    setobjs2s(L, p, p-1);
  L->top++;  /* stack space pre-allocated by the caller */
  setobj2s(L, func, tm);  /* metamethod is the new function to be called */
}