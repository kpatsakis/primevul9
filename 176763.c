static char *my_fgets(char *buf, size_t buf_len, FILE *stream) {
  char *ret = fgets(buf, buf_len, stream);

  if (ret) {
    size_t len = strlen(ret);
    if (len > 0 && ret[len - 1] == '\n') ret[len - 1] = '\0';
  }

  return ret;
}