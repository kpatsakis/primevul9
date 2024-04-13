l_noret luaD_throw (lua_State *L, int errcode) {
  if (L->errorJmp) {  /* thread has an error handler? */
    L->errorJmp->status = errcode;  /* set status */
    LUAI_THROW(L, L->errorJmp);  /* jump to it */
  }
  else {  /* thread has no error handler */
    global_State *g = G(L);
    errcode = luaF_close(L, L->stack, errcode);  /* close all upvalues */
    L->status = cast_byte(errcode);  /* mark it as dead */
    if (g->mainthread->errorJmp) {  /* main thread has a handler? */
      setobjs2s(L, g->mainthread->top++, L->top - 1);  /* copy error obj. */
      luaD_throw(g->mainthread, errcode);  /* re-throw in main thread */
    }
    else {  /* no handler at all; abort */
      if (g->panic) {  /* panic function? */
        luaD_seterrorobj(L, errcode, L->top);  /* assume EXTRA_STACK */
        if (L->ci->top < L->top)
          L->ci->top = L->top;  /* pushing msg. can break this invariant */
        lua_unlock(L);
        g->panic(L);  /* call panic function (last chance to jump out) */
      }
      abort();
    }
  }
}