void luaD_shrinkstack (lua_State *L) {
  int inuse = stackinuse(L);
  int goodsize = inuse + BASIC_STACK_SIZE;
  if (goodsize > LUAI_MAXSTACK)
    goodsize = LUAI_MAXSTACK;  /* respect stack limit */
  /* if thread is currently not handling a stack overflow and its
     good size is smaller than current size, shrink its stack */
  if (inuse <= (LUAI_MAXSTACK - EXTRA_STACK) && goodsize < L->stacksize)
    luaD_reallocstack(L, goodsize, 0);  /* ok if that fails */
  else  /* don't change stack */
    condmovestack(L,{},{});  /* (change only for debugging) */
  luaE_shrinkCI(L);  /* shrink CI list */
}