static void unroll (lua_State *L, void *ud) {
  CallInfo *ci;
  if (ud != NULL)  /* error status? */
    finishCcall(L, *(int *)ud);  /* finish 'lua_pcallk' callee */
  while ((ci = L->ci) != &L->base_ci) {  /* something in the stack */
    if (!isLua(ci))  /* C function? */
      finishCcall(L, LUA_YIELD);  /* complete its execution */
    else {  /* Lua function */
      luaV_finishOp(L);  /* finish interrupted instruction */
      luaV_execute(L, ci);  /* execute down to higher C 'boundary' */
    }
  }
}