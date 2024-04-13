void destroyURL(struct URL *url) {
  if (url) {
    free(url->protocol);
    free(url->user);
    free(url->password);
    free(url->host);
    free(url->path);
    free(url->pathinfo);
    free(url->query);
    free(url->anchor);
    free(url->url);
    destroyHashMap(&url->args);
  }
}