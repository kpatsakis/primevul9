static void moveresults (lua_State *L, StkId res, int nres, int wanted) {
  StkId firstresult;
  int i;
  switch (wanted) {  /* handle typical cases separately */
    case 0:  /* no values needed */
      L->top = res;
      return;
    case 1:  /* one value needed */
      if (nres == 0)   /* no results? */
        setnilvalue(s2v(res));  /* adjust with nil */
      else
        setobjs2s(L, res, L->top - nres);  /* move it to proper place */
      L->top = res + 1;
      return;
    case LUA_MULTRET:
      wanted = nres;  /* we want all results */
      break;
    default:  /* multiple results (or to-be-closed variables) */
      if (hastocloseCfunc(wanted)) {  /* to-be-closed variables? */
        ptrdiff_t savedres = savestack(L, res);
        luaF_close(L, res, LUA_OK);  /* may change the stack */
        res = restorestack(L, savedres);
        wanted = codeNresults(wanted);  /* correct value */
        if (wanted == LUA_MULTRET)
          wanted = nres;
      }
      break;
  }
  firstresult = L->top - nres;  /* index of first result */
  /* move all results to correct place */
  for (i = 0; i < nres && i < wanted; i++)
    setobjs2s(L, res + i, firstresult + i);
  for (; i < wanted; i++)  /* complete wanted number of results */
    setnilvalue(s2v(res + i));
  L->top = res + wanted;  /* top points after the last result */
}