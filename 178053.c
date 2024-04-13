static CURLcode parse_remote_port(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  char *portptr;
  char endbracket;

  /* Note that at this point, the IPv6 address cannot contain any scope
     suffix as that has already been removed in the parseurlandfillconn()
     function */
  if((1 == sscanf(conn->host.name, "[%*45[0123456789abcdefABCDEF:.]%c",
                  &endbracket)) &&
     (']' == endbracket)) {
    /* this is a RFC2732-style specified IP-address */
    conn->bits.ipv6_ip = TRUE;

    conn->host.name++; /* skip over the starting bracket */
    portptr = strchr(conn->host.name, ']');
    if(portptr) {
      *portptr++ = '\0'; /* zero terminate, killing the bracket */
      if(*portptr) {
        if (*portptr != ':') {
          failf(data, "IPv6 closing bracket followed by '%c'", *portptr);
          return CURLE_URL_MALFORMAT;
        }
      }
      else
        portptr = NULL; /* no port number available */
    }
  }
  else {
#ifdef ENABLE_IPV6
    struct in6_addr in6;
    if(Curl_inet_pton(AF_INET6, conn->host.name, &in6) > 0) {
      /* This is a numerical IPv6 address, meaning this is a wrongly formatted
         URL */
      failf(data, "IPv6 numerical address used in URL without brackets");
      return CURLE_URL_MALFORMAT;
    }
#endif

    portptr = strchr(conn->host.name, ':');
  }

  if(data->set.use_port && data->state.allow_port) {
    /* if set, we use this and ignore the port possibly given in the URL */
    conn->remote_port = (unsigned short)data->set.use_port;
    if(portptr)
      *portptr = '\0'; /* cut off the name there anyway - if there was a port
                      number - since the port number is to be ignored! */
    if(conn->bits.httpproxy) {
      /* we need to create new URL with the new port number */
      char *url;
      char type[12]="";

      if(conn->bits.type_set)
        snprintf(type, sizeof(type), ";type=%c",
                 data->set.prefer_ascii?'A':
                 (data->set.ftp_list_only?'D':'I'));

      /*
       * This synthesized URL isn't always right--suffixes like ;type=A are
       * stripped off. It would be better to work directly from the original
       * URL and simply replace the port part of it.
       */
      url = aprintf("%s://%s%s%s:%hu%s%s%s", conn->given->scheme,
                    conn->bits.ipv6_ip?"[":"", conn->host.name,
                    conn->bits.ipv6_ip?"]":"", conn->remote_port,
                    data->state.slash_removed?"/":"", data->state.path,
                    type);
      if(!url)
        return CURLE_OUT_OF_MEMORY;

      if(data->change.url_alloc) {
        Curl_safefree(data->change.url);
        data->change.url_alloc = FALSE;
      }

      data->change.url = url;
      data->change.url_alloc = TRUE;
    }
  }
  else if(portptr) {
    /* no CURLOPT_PORT given, extract the one from the URL */

    char *rest;
    long port;

    port = strtol(portptr + 1, &rest, 10);  /* Port number must be decimal */

    if((port < 0) || (port > 0xffff)) {
      /* Single unix standard says port numbers are 16 bits long */
      failf(data, "Port number out of range");
      return CURLE_URL_MALFORMAT;
    }

    if(rest[0]) {
      failf(data, "Port number ended with '%c'", rest[0]);
      return CURLE_URL_MALFORMAT;
    }

    if(rest != &portptr[1]) {
      *portptr = '\0'; /* cut off the name there */
      conn->remote_port = curlx_ultous(port);
    }
    else {
      /* Browser behavior adaptation. If there's a colon with no digits after,
         just cut off the name there which makes us ignore the colon and just
         use the default port. Firefox and Chrome both do that. */
      *portptr = '\0';
    }
  }

  /* only if remote_port was not already parsed off the URL we use the
     default port number */
  if(conn->remote_port < 0)
    conn->remote_port = (unsigned short)conn->given->defport;

  return CURLE_OK;
}