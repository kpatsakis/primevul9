void initURL(struct URL *url, const struct HttpConnection *http,
             const char *buf, int len) {
  url->protocol              = strdup(httpGetProtocol(http));
  url->user                  = NULL;
  url->password              = NULL;
  url->host                  = strdup(httpGetHost(http));
  url->port                  = httpGetPort(http);
  url->path                  = strdup(httpGetPath(http));
  url->pathinfo              = strdup(httpGetPathInfo(http));
  url->query                 = strdup(httpGetQuery(http));
  url->anchor                = NULL;
  url->url                   = NULL;
  initHashMap(&url->args, urlDestroyHashMapEntry, NULL);
  if (!strcmp(http->method, "GET")) {
    check(url->query);
    urlParseQueryString(&url->args, url->query, strlen(url->query));
  } else if (!strcmp(http->method, "POST")) {
    urlParsePostBody(url, http, buf, len);
  }
}