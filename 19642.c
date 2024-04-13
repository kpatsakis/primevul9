static int newlabelentry (LexState *ls, Labellist *l, TString *name,
                          int line, int pc) {
  int n = l->n;
  luaM_growvector(ls->L, l->arr, n, l->size,
                  Labeldesc, SHRT_MAX, "labels/gotos");
  l->arr[n].name = name;
  l->arr[n].line = line;
  l->arr[n].nactvar = ls->fs->nactvar;
  l->arr[n].close = 0;
  l->arr[n].pc = pc;
  l->n = n + 1;
  return n;
}