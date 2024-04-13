void luaD_hook (lua_State *L, int event, int line,
                              int ftransfer, int ntransfer) {
  lua_Hook hook = L->hook;
  if (hook && L->allowhook) {  /* make sure there is a hook */
    int mask = CIST_HOOKED;
    CallInfo *ci = L->ci;
    ptrdiff_t top = savestack(L, L->top);
    ptrdiff_t ci_top = savestack(L, ci->top);
    lua_Debug ar;
    ar.event = event;
    ar.currentline = line;
    ar.i_ci = ci;
    if (ntransfer != 0) {
      mask |= CIST_TRAN;  /* 'ci' has transfer information */
      ci->u2.transferinfo.ftransfer = ftransfer;
      ci->u2.transferinfo.ntransfer = ntransfer;
    }
    luaD_checkstack(L, LUA_MINSTACK);  /* ensure minimum stack size */
    if (L->top + LUA_MINSTACK > ci->top)
      ci->top = L->top + LUA_MINSTACK;
    L->allowhook = 0;  /* cannot call hooks inside a hook */
    ci->callstatus |= mask;
    lua_unlock(L);
    (*hook)(L, &ar);
    lua_lock(L);
    lua_assert(!L->allowhook);
    L->allowhook = 1;
    ci->top = restorestack(L, ci_top);
    L->top = restorestack(L, top);
    ci->callstatus &= ~mask;
  }
}