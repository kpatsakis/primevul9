void deleteURL(struct URL *url) {
  destroyURL(url);
  free(url);
}