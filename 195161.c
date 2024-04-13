bool EmbFile::save(const char *path) {
  FILE *f;
  bool ret;

  if (!(f = fopen(path, "wb"))) {
    return false;
  }
  ret = save2(f);
  fclose(f);
  return ret;
}