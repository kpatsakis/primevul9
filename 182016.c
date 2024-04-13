void luaD_pretailcall (lua_State *L, CallInfo *ci, StkId func, int narg1) {
  Proto *p = clLvalue(s2v(func))->p;
  int fsize = p->maxstacksize;  /* frame size */
  int nfixparams = p->numparams;
  int i;
  for (i = 0; i < narg1; i++)  /* move down function and arguments */
    setobjs2s(L, ci->func + i, func + i);
  checkstackGC(L, fsize);
  func = ci->func;  /* moved-down function */
  for (; narg1 <= nfixparams; narg1++)
    setnilvalue(s2v(func + narg1));  /* complete missing arguments */
  ci->top = func + 1 + fsize;  /* top for new function */
  lua_assert(ci->top <= L->stack_last);
  ci->u.l.savedpc = p->code;  /* starting point */
  ci->callstatus |= CIST_TAIL;
  L->top = func + narg1;  /* set top */
}