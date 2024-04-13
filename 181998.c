static void correctstack (lua_State *L, StkId oldstack, StkId newstack) {
  CallInfo *ci;
  UpVal *up;
  if (oldstack == newstack)
    return;  /* stack address did not change */
  L->top = (L->top - oldstack) + newstack;
  for (up = L->openupval; up != NULL; up = up->u.open.next)
    up->v = s2v((uplevel(up) - oldstack) + newstack);
  for (ci = L->ci; ci != NULL; ci = ci->previous) {
    ci->top = (ci->top - oldstack) + newstack;
    ci->func = (ci->func - oldstack) + newstack;
    if (isLua(ci))
      ci->u.l.trap = 1;  /* signal to update 'trap' in 'luaV_execute' */
  }
}