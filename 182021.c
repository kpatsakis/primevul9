int luaD_growstack (lua_State *L, int n, int raiseerror) {
  int size = L->stacksize;
  int newsize = 2 * size;  /* tentative new size */
  if (unlikely(size > LUAI_MAXSTACK)) {  /* need more space after extra size? */
    if (raiseerror)
      luaD_throw(L, LUA_ERRERR);  /* error inside message handler */
    else return 0;
  }
  else {
    int needed = cast_int(L->top - L->stack) + n + EXTRA_STACK;
    if (newsize > LUAI_MAXSTACK)  /* cannot cross the limit */
      newsize = LUAI_MAXSTACK;
    if (newsize < needed)  /* but must respect what was asked for */
      newsize = needed;
    if (unlikely(newsize > LUAI_MAXSTACK)) {  /* stack overflow? */
      /* add extra size to be able to handle the error message */
      luaD_reallocstack(L, ERRORSTACKSIZE, raiseerror);
      if (raiseerror)
        luaG_runerror(L, "stack overflow");
      else return 0;
    }
  }  /* else no errors */
  return luaD_reallocstack(L, newsize, raiseerror);
}