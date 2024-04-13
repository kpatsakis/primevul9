struct URL *newURL(const struct HttpConnection *http,
                   const char *buf, int len) {
  struct URL *url;
  check(url = malloc(sizeof(struct URL)));
  initURL(url, http, buf, len);
  return url;
}