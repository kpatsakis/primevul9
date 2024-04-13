static UnOpr getunopr (int op) {
  switch (op) {
    case TK_NOT: return OPR_NOT;
    case '-': return OPR_MINUS;
    case '~': return OPR_BNOT;
    case '#': return OPR_LEN;
    default: return OPR_NOUNOPR;
  }
}