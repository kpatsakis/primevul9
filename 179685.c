const char *urlGetURL(struct URL *url) {
  if (!url->url) {
    const char *host           = urlGetHost(url);
    int s_size                 = 8 + strlen(host) + 25 + strlen(url->path);
    check(*(char **)&url->url  = malloc(s_size + 1));
    *url->url                  = '\000';
    strncat(url->url, url->protocol, s_size);
    strncat(url->url, "://", s_size);
    strncat(url->url, host, s_size);
    if (url->port != (strcmp(url->protocol, "http") ? 443 : 80)) {
      snprintf(strrchr(url->url, '\000'), 25, ":%d", url->port);
    }
    strncat(url->url, url->path, s_size);
  }
  return url->url;
}