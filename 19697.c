int luaY_nvarstack (FuncState *fs) {
  return reglevel(fs, fs->nactvar);
}