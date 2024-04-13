static void resume (lua_State *L, void *ud) {
  int n = *(cast(int*, ud));  /* number of arguments */
  StkId firstArg = L->top - n;  /* first argument */
  CallInfo *ci = L->ci;
  if (L->status == LUA_OK) {  /* starting a coroutine? */
    luaD_call(L, firstArg - 1, LUA_MULTRET);
  }
  else {  /* resuming from previous yield */
    lua_assert(L->status == LUA_YIELD);
    L->status = LUA_OK;  /* mark that it is running (again) */
    if (isLua(ci))  /* yielded inside a hook? */
      luaV_execute(L, ci);  /* just continue running Lua code */
    else {  /* 'common' yield */
      if (ci->u.c.k != NULL) {  /* does it have a continuation function? */
        lua_unlock(L);
        n = (*ci->u.c.k)(L, LUA_YIELD, ci->u.c.ctx); /* call continuation */
        lua_lock(L);
        api_checknelems(L, n);
      }
      luaD_poscall(L, ci, n);  /* finish 'luaD_call' */
    }
    unroll(L, NULL);  /* run continuation */
  }
}