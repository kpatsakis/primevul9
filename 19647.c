static void check_conflict (LexState *ls, struct LHS_assign *lh, expdesc *v) {
  FuncState *fs = ls->fs;
  int extra = fs->freereg;  /* eventual position to save local variable */
  int conflict = 0;
  for (; lh; lh = lh->prev) {  /* check all previous assignments */
    if (vkisindexed(lh->v.k)) {  /* assignment to table field? */
      if (lh->v.k == VINDEXUP) {  /* is table an upvalue? */
        if (v->k == VUPVAL && lh->v.u.ind.t == v->u.info) {
          conflict = 1;  /* table is the upvalue being assigned now */
          lh->v.k = VINDEXSTR;
          lh->v.u.ind.t = extra;  /* assignment will use safe copy */
        }
      }
      else {  /* table is a register */
        if (v->k == VLOCAL && lh->v.u.ind.t == v->u.var.ridx) {
          conflict = 1;  /* table is the local being assigned now */
          lh->v.u.ind.t = extra;  /* assignment will use safe copy */
        }
        /* is index the local being assigned? */
        if (lh->v.k == VINDEXED && v->k == VLOCAL &&
            lh->v.u.ind.idx == v->u.var.ridx) {
          conflict = 1;
          lh->v.u.ind.idx = extra;  /* previous assignment will use safe copy */
        }
      }
    }
  }
  if (conflict) {
    /* copy upvalue/local value to a temporary (in position 'extra') */
    if (v->k == VLOCAL)
      luaK_codeABC(fs, OP_MOVE, extra, v->u.var.ridx, 0);
    else
      luaK_codeABC(fs, OP_GETUPVAL, extra, v->u.info, 0);
    luaK_reserveregs(fs, 1);
  }
}