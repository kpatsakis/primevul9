static void restassign (LexState *ls, struct LHS_assign *lh, int nvars) {
  expdesc e;
  check_condition(ls, vkisvar(lh->v.k), "syntax error");
  check_readonly(ls, &lh->v);
  if (testnext(ls, ',')) {  /* restassign -> ',' suffixedexp restassign */
    struct LHS_assign nv;
    nv.prev = lh;
    suffixedexp(ls, &nv.v);
    if (!vkisindexed(nv.v.k))
      check_conflict(ls, lh, &nv.v);
    enterlevel(ls);  /* control recursion depth */
    restassign(ls, &nv, nvars+1);
    leavelevel(ls);
  }
  else {  /* restassign -> '=' explist */
    int nexps;
    checknext(ls, '=');
    nexps = explist(ls, &e);
    if (nexps != nvars)
      adjust_assign(ls, nvars, nexps, &e);
    else {
      luaK_setoneret(ls->fs, &e);  /* close last expression */
      luaK_storevar(ls->fs, &lh->v, &e);
      return;  /* avoid default */
    }
  }
  init_exp(&e, VNONRELOC, ls->fs->freereg-1);  /* default assignment */
  luaK_storevar(ls->fs, &lh->v, &e);
}