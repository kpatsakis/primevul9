CURLcode Curl_http(struct connectdata *conn, bool *done)
{
  struct SessionHandle *data = conn->data;
  CURLcode result = CURLE_OK;
  struct HTTP *http;
  const char *ppath = data->state.path;
  bool paste_ftp_userpwd = FALSE;
  char ftp_typecode[sizeof("/;type=?")] = "";
  const char *host = conn->host.name;
  const char *te = ""; /* transfer-encoding */
  const char *ptr;
  const char *request;
  Curl_HttpReq httpreq = data->set.httpreq;
#if !defined(CURL_DISABLE_COOKIES)
  char *addcookies = NULL;
#endif
  curl_off_t included_body = 0;
  const char *httpstring;
  Curl_send_buffer *req_buffer;
  curl_off_t postsize = 0; /* curl_off_t to handle large file sizes */
  int seekerr = CURL_SEEKFUNC_OK;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that is not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *done = TRUE;

  if(conn->httpversion < 20) { /* unless the connection is re-used and already
                                  http2 */
    switch(conn->negnpn) {
    case CURL_HTTP_VERSION_2_0:
      conn->httpversion = 20; /* we know we're on HTTP/2 now */
      result = Curl_http2_init(conn);
      if(result)
        return result;

      result = Curl_http2_setup(conn);
      if(result)
        return result;

      result = Curl_http2_switched(conn, NULL, 0);
      if(result)
        return result;
      break;
    case CURL_HTTP_VERSION_1_1:
      /* continue with HTTP/1.1 when explicitly requested */
      break;
    default:
      /* and as fallback */
      break;
    }
  }
  else {
    /* prepare for a http2 request */
    result = Curl_http2_setup(conn);
    if(result)
      return result;
  }

  http = data->req.protop;

  if(!data->state.this_is_a_follow) {
    /* Free to avoid leaking memory on multiple requests*/
    free(data->state.first_host);

    data->state.first_host = strdup(conn->host.name);
    if(!data->state.first_host)
      return CURLE_OUT_OF_MEMORY;
  }
  http->writebytecount = http->readbytecount = 0;

  if((conn->handler->protocol&(PROTO_FAMILY_HTTP|CURLPROTO_FTP)) &&
     data->set.upload) {
    httpreq = HTTPREQ_PUT;
  }

  /* Now set the 'request' pointer to the proper request string */
  if(data->set.str[STRING_CUSTOMREQUEST])
    request = data->set.str[STRING_CUSTOMREQUEST];
  else {
    if(data->set.opt_no_body)
      request = "HEAD";
    else {
      DEBUGASSERT((httpreq > HTTPREQ_NONE) && (httpreq < HTTPREQ_LAST));
      switch(httpreq) {
      case HTTPREQ_POST:
      case HTTPREQ_POST_FORM:
        request = "POST";
        break;
      case HTTPREQ_PUT:
        request = "PUT";
        break;
      default: /* this should never happen */
      case HTTPREQ_GET:
        request = "GET";
        break;
      case HTTPREQ_HEAD:
        request = "HEAD";
        break;
      }
    }
  }

  /* The User-Agent string might have been allocated in url.c already, because
     it might have been used in the proxy connect, but if we have got a header
     with the user-agent string specified, we erase the previously made string
     here. */
  if(Curl_checkheaders(conn, "User-Agent:")) {
    free(conn->allocptr.uagent);
    conn->allocptr.uagent=NULL;
  }

  /* setup the authentication headers */
  result = Curl_http_output_auth(conn, request, ppath, FALSE);
  if(result)
    return result;

  if((data->state.authhost.multi || data->state.authproxy.multi) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    conn->bits.authneg = TRUE;
  }
  else
    conn->bits.authneg = FALSE;

  Curl_safefree(conn->allocptr.ref);
  if(data->change.referer && !Curl_checkheaders(conn, "Referer:")) {
    conn->allocptr.ref = aprintf("Referer: %s\r\n", data->change.referer);
    if(!conn->allocptr.ref)
      return CURLE_OUT_OF_MEMORY;
  }
  else
    conn->allocptr.ref = NULL;

#if !defined(CURL_DISABLE_COOKIES)
  if(data->set.str[STRING_COOKIE] && !Curl_checkheaders(conn, "Cookie:"))
    addcookies = data->set.str[STRING_COOKIE];
#endif

  if(!Curl_checkheaders(conn, "Accept-Encoding:") &&
     data->set.str[STRING_ENCODING]) {
    Curl_safefree(conn->allocptr.accept_encoding);
    conn->allocptr.accept_encoding =
      aprintf("Accept-Encoding: %s\r\n", data->set.str[STRING_ENCODING]);
    if(!conn->allocptr.accept_encoding)
      return CURLE_OUT_OF_MEMORY;
  }

#ifdef HAVE_LIBZ
  /* we only consider transfer-encoding magic if libz support is built-in */

  if(!Curl_checkheaders(conn, "TE:") &&
     data->set.http_transfer_encoding) {
    /* When we are to insert a TE: header in the request, we must also insert
       TE in a Connection: header, so we need to merge the custom provided
       Connection: header and prevent the original to get sent. Note that if
       the user has inserted his/hers own TE: header we don't do this magic
       but then assume that the user will handle it all! */
    char *cptr = Curl_checkheaders(conn, "Connection:");
#define TE_HEADER "TE: gzip\r\n"

    Curl_safefree(conn->allocptr.te);

    /* Create the (updated) Connection: header */
    conn->allocptr.te = cptr? aprintf("%s, TE\r\n" TE_HEADER, cptr):
      strdup("Connection: TE\r\n" TE_HEADER);

    if(!conn->allocptr.te)
      return CURLE_OUT_OF_MEMORY;
  }
#endif

  if(conn->httpversion == 20)
    /* In HTTP2 forbids Transfer-Encoding: chunked */
    ptr = NULL;
  else {
    ptr = Curl_checkheaders(conn, "Transfer-Encoding:");
    if(ptr) {
      /* Some kind of TE is requested, check if 'chunked' is chosen */
      data->req.upload_chunky =
        Curl_compareheader(ptr, "Transfer-Encoding:", "chunked");
    }
    else {
      if((conn->handler->protocol&PROTO_FAMILY_HTTP) &&
         data->set.upload &&
         (data->state.infilesize == -1)) {
        if(conn->bits.authneg)
          /* don't enable chunked during auth neg */
          ;
        else if(use_http_1_1plus(data, conn)) {
          /* HTTP, upload, unknown file size and not HTTP 1.0 */
          data->req.upload_chunky = TRUE;
        }
        else {
          failf(data, "Chunky upload is not supported by HTTP 1.0");
          return CURLE_UPLOAD_FAILED;
        }
      }
      else {
        /* else, no chunky upload */
        data->req.upload_chunky = FALSE;
      }

      if(data->req.upload_chunky)
        te = "Transfer-Encoding: chunked\r\n";
    }
  }

  Curl_safefree(conn->allocptr.host);

  ptr = Curl_checkheaders(conn, "Host:");
  if(ptr && (!data->state.this_is_a_follow ||
             Curl_raw_equal(data->state.first_host, conn->host.name))) {
#if !defined(CURL_DISABLE_COOKIES)
    /* If we have a given custom Host: header, we extract the host name in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the host name
       is the same as the first one! */
    char *cookiehost = Curl_copy_header_value(ptr);
    if(!cookiehost)
      return CURLE_OUT_OF_MEMORY;
    if(!*cookiehost)
      /* ignore empty data */
      free(cookiehost);
    else {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      int startsearch = 0;
      if(*cookiehost == '[') {
        char *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot simply increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        if(closingbracket)
          *closingbracket = 0;
      }
      else {
        char *colon = strchr(cookiehost + startsearch, ':');
        if(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      Curl_safefree(conn->allocptr.cookiehost);
      conn->allocptr.cookiehost = cookiehost;
    }
#endif

    if(strcmp("Host:", ptr)) {
      conn->allocptr.host = aprintf("%s\r\n", ptr);
      if(!conn->allocptr.host)
        return CURLE_OUT_OF_MEMORY;
    }
    else
      /* when clearing the header */
      conn->allocptr.host = NULL;
  }
  else {
    /* When building Host: headers, we must put the host name within
       [brackets] if the host name is a plain IPv6-address. RFC2732-style. */

    if(((conn->given->protocol&CURLPROTO_HTTPS) &&
        (conn->remote_port == PORT_HTTPS)) ||
       ((conn->given->protocol&CURLPROTO_HTTP) &&
        (conn->remote_port == PORT_HTTP)) )
      /* if(HTTPS on port 443) OR (HTTP on port 80) then don't include
         the port number in the host string */
      conn->allocptr.host = aprintf("Host: %s%s%s\r\n",
                                    conn->bits.ipv6_ip?"[":"",
                                    host,
                                    conn->bits.ipv6_ip?"]":"");
    else
      conn->allocptr.host = aprintf("Host: %s%s%s:%hu\r\n",
                                    conn->bits.ipv6_ip?"[":"",
                                    host,
                                    conn->bits.ipv6_ip?"]":"",
                                    conn->remote_port);

    if(!conn->allocptr.host)
      /* without Host: we can't make a nice request */
      return CURLE_OUT_OF_MEMORY;
  }

#ifndef CURL_DISABLE_PROXY
  if(conn->bits.httpproxy && !conn->bits.tunnel_proxy)  {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL. But if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the encoded host name! */
    if(conn->host.dispname != conn->host.name) {
      char *url = data->change.url;
      ptr = strstr(url, conn->host.dispname);
      if(ptr) {
        /* This is where the display name starts in the URL, now replace this
           part with the encoded name. TODO: This method of replacing the host
           name is rather crude as I believe there's a slight risk that the
           user has entered a user name or password that contain the host name
           string. */
        size_t currlen = strlen(conn->host.dispname);
        size_t newlen = strlen(conn->host.name);
        size_t urllen = strlen(url);

        char *newurl;

        newurl = malloc(urllen + newlen - currlen + 1);
        if(newurl) {
          /* copy the part before the host name */
          memcpy(newurl, url, ptr - url);
          /* append the new host name instead of the old */
          memcpy(newurl + (ptr - url), conn->host.name, newlen);
          /* append the piece after the host name */
          memcpy(newurl + newlen + (ptr - url),
                 ptr + currlen, /* copy the trailing zero byte too */
                 urllen - (ptr-url) - currlen + 1);
          if(data->change.url_alloc) {
            Curl_safefree(data->change.url);
            data->change.url_alloc = FALSE;
          }
          data->change.url = newurl;
          data->change.url_alloc = TRUE;
        }
        else
          return CURLE_OUT_OF_MEMORY;
      }
    }
    ppath = data->change.url;
    if(checkprefix("ftp://", ppath)) {
      if(data->set.proxy_transfer_mode) {
        /* when doing ftp, append ;type=<a|i> if not present */
        char *type = strstr(ppath, ";type=");
        if(type && type[6] && type[7] == 0) {
          switch (Curl_raw_toupper(type[6])) {
          case 'A':
          case 'D':
          case 'I':
            break;
          default:
            type = NULL;
          }
        }
        if(!type) {
          char *p = ftp_typecode;
          /* avoid sending invalid URLs like ftp://example.com;type=i if the
           * user specified ftp://example.com without the slash */
          if(!*data->state.path && ppath[strlen(ppath) - 1] != '/') {
            *p++ = '/';
          }
          snprintf(p, sizeof(ftp_typecode) - 1, ";type=%c",
                   data->set.prefer_ascii ? 'a' : 'i');
        }
      }
      if(conn->bits.user_passwd && !conn->bits.userpwd_in_url)
        paste_ftp_userpwd = TRUE;
    }
  }
#endif /* CURL_DISABLE_PROXY */

  if(HTTPREQ_POST_FORM == httpreq) {
    /* we must build the whole post sequence first, so that we have a size of
       the whole transfer before we start to send it */
    result = Curl_getformdata(data, &http->sendit, data->set.httppost,
                              Curl_checkheaders(conn, "Content-Type:"),
                              &http->postsize);
    if(result)
      return result;
  }

  http->p_accept = Curl_checkheaders(conn, "Accept:")?NULL:"Accept: */*\r\n";

  if(( (HTTPREQ_POST == httpreq) ||
       (HTTPREQ_POST_FORM == httpreq) ||
       (HTTPREQ_PUT == httpreq) ) &&
     data->state.resume_from) {
    /**********************************************************************
     * Resuming upload in HTTP means that we PUT or POST and that we have
     * got a resume_from value set. The resume value has already created
     * a Range: header that will be passed along. We need to "fast forward"
     * the file the given number of bytes and decrease the assume upload
     * file size before we continue this venture in the dark lands of HTTP.
     *********************************************************************/

    if(data->state.resume_from < 0 ) {
      /*
       * This is meant to get the size of the present remote-file by itself.
       * We don't support this now. Bail out!
       */
      data->state.resume_from = 0;
    }

    if(data->state.resume_from && !data->state.this_is_a_follow) {
      /* do we still game? */

      /* Now, let's read off the proper amount of bytes from the
         input. */
      if(conn->seek_func) {
        seekerr = conn->seek_func(conn->seek_client, data->state.resume_from,
                                  SEEK_SET);
      }

      if(seekerr != CURL_SEEKFUNC_OK) {
        if(seekerr != CURL_SEEKFUNC_CANTSEEK) {
          failf(data, "Could not seek stream");
          return CURLE_READ_ERROR;
        }
        /* when seekerr == CURL_SEEKFUNC_CANTSEEK (can't seek to offset) */
        else {
          curl_off_t passed=0;
          do {
            size_t readthisamountnow =
              (data->state.resume_from - passed > CURL_OFF_T_C(BUFSIZE)) ?
              BUFSIZE : curlx_sotouz(data->state.resume_from - passed);

            size_t actuallyread =
              data->set.fread_func(data->state.buffer, 1, readthisamountnow,
                                   data->set.in);

            passed += actuallyread;
            if((actuallyread == 0) || (actuallyread > readthisamountnow)) {
              /* this checks for greater-than only to make sure that the
                 CURL_READFUNC_ABORT return code still aborts */
              failf(data, "Could only read %" CURL_FORMAT_CURL_OFF_T
                    " bytes from the input", passed);
              return CURLE_READ_ERROR;
            }
          } while(passed < data->state.resume_from);
        }
      }

      /* now, decrease the size of the read */
      if(data->state.infilesize>0) {
        data->state.infilesize -= data->state.resume_from;

        if(data->state.infilesize <= 0) {
          failf(data, "File already completely uploaded");
          return CURLE_PARTIAL_FILE;
        }
      }
      /* we've passed, proceed as normal */
    }
  }
  if(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we're downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    if(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(conn, "Range:")) {
      /* if a line like this was already allocated, free the previous one */
      free(conn->allocptr.rangeline);
      conn->allocptr.rangeline = aprintf("Range: bytes=%s\r\n",
                                         data->state.range);
    }
    else if((httpreq != HTTPREQ_GET) &&
            !Curl_checkheaders(conn, "Content-Range:")) {

      /* if a line like this was already allocated, free the previous one */
      free(conn->allocptr.rangeline);

      if(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we don't know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        conn->allocptr.rangeline =
          aprintf("Content-Range: bytes 0-%" CURL_FORMAT_CURL_OFF_T
                  "/%" CURL_FORMAT_CURL_OFF_T "\r\n",
                  data->state.infilesize - 1, data->state.infilesize);

      }
      else if(data->state.resume_from) {
        /* This is because "resume" was selected */
        curl_off_t total_expected_size=
          data->state.resume_from + data->state.infilesize;
        conn->allocptr.rangeline =
          aprintf("Content-Range: bytes %s%" CURL_FORMAT_CURL_OFF_T
                  "/%" CURL_FORMAT_CURL_OFF_T "\r\n",
                  data->state.range, total_expected_size-1,
                  total_expected_size);
      }
      else {
        /* Range was selected and then we just pass the incoming range and
           append total size */
        conn->allocptr.rangeline =
          aprintf("Content-Range: bytes %s/%" CURL_FORMAT_CURL_OFF_T "\r\n",
                  data->state.range, data->state.infilesize);
      }
      if(!conn->allocptr.rangeline)
        return CURLE_OUT_OF_MEMORY;
    }
  }

  /* Use 1.1 unless the user specifically asked for 1.0 or the server only
     supports 1.0 */
  httpstring= use_http_1_1plus(data, conn)?"1.1":"1.0";

  /* initialize a dynamic send-buffer */
  req_buffer = Curl_add_buffer_init();

  if(!req_buffer)
    return CURLE_OUT_OF_MEMORY;

  /* add the main request stuff */
  /* GET/HEAD/POST/PUT */
  result = Curl_add_bufferf(req_buffer, "%s ", request);
  if(result)
    return result;

  /* url */
  if(paste_ftp_userpwd)
    result = Curl_add_bufferf(req_buffer, "ftp://%s:%s@%s",
                              conn->user, conn->passwd,
                              ppath + sizeof("ftp://") - 1);
  else
    result = Curl_add_buffer(req_buffer, ppath, strlen(ppath));
  if(result)
    return result;

  result =
    Curl_add_bufferf(req_buffer,
                     "%s" /* ftp typecode (;type=x) */
                     " HTTP/%s\r\n" /* HTTP version */
                     "%s" /* host */
                     "%s" /* proxyuserpwd */
                     "%s" /* userpwd */
                     "%s" /* range */
                     "%s" /* user agent */
                     "%s" /* accept */
                     "%s" /* TE: */
                     "%s" /* accept-encoding */
                     "%s" /* referer */
                     "%s" /* Proxy-Connection */
                     "%s",/* transfer-encoding */

                     ftp_typecode,
                     httpstring,
                     (conn->allocptr.host?conn->allocptr.host:""),
                     conn->allocptr.proxyuserpwd?
                     conn->allocptr.proxyuserpwd:"",
                     conn->allocptr.userpwd?conn->allocptr.userpwd:"",
                     (data->state.use_range && conn->allocptr.rangeline)?
                     conn->allocptr.rangeline:"",
                     (data->set.str[STRING_USERAGENT] &&
                      *data->set.str[STRING_USERAGENT] &&
                      conn->allocptr.uagent)?
                     conn->allocptr.uagent:"",
                     http->p_accept?http->p_accept:"",
                     conn->allocptr.te?conn->allocptr.te:"",
                     (data->set.str[STRING_ENCODING] &&
                      *data->set.str[STRING_ENCODING] &&
                      conn->allocptr.accept_encoding)?
                     conn->allocptr.accept_encoding:"",
                     (data->change.referer && conn->allocptr.ref)?
                     conn->allocptr.ref:"" /* Referer: <data> */,
                     (conn->bits.httpproxy &&
                      !conn->bits.tunnel_proxy &&
                      !Curl_checkProxyheaders(conn, "Proxy-Connection:"))?
                     "Proxy-Connection: Keep-Alive\r\n":"",
                     te
      );

  /*
   * Free userpwd for Negotiate/NTLM. Cannot reuse as it is associated with
   * the connection and shouldn't be repeated over it either.
   */
  switch (data->state.authhost.picked) {
  case CURLAUTH_NEGOTIATE:
  case CURLAUTH_NTLM:
  case CURLAUTH_NTLM_WB:
    Curl_safefree(conn->allocptr.userpwd);
    break;
  }

  /*
   * Same for proxyuserpwd
   */
  switch (data->state.authproxy.picked) {
  case CURLAUTH_NEGOTIATE:
  case CURLAUTH_NTLM:
  case CURLAUTH_NTLM_WB:
    Curl_safefree(conn->allocptr.proxyuserpwd);
    break;
  }

  if(result)
    return result;

  if(!(conn->handler->flags&PROTOPT_SSL) &&
     conn->httpversion != 20 &&
     (data->set.httpversion == CURL_HTTP_VERSION_2_0)) {
    /* append HTTP2 upgrade magic stuff to the HTTP request if it isn't done
       over SSL */
    result = Curl_http2_request_upgrade(req_buffer, conn);
    if(result)
      return result;
  }

#if !defined(CURL_DISABLE_COOKIES)
  if(data->cookies || addcookies) {
    struct Cookie *co=NULL; /* no cookies from start */
    int count=0;

    if(data->cookies) {
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      co = Curl_cookie_getlist(data->cookies,
                               conn->allocptr.cookiehost?
                               conn->allocptr.cookiehost:host,
                               data->state.path,
                               (conn->handler->protocol&CURLPROTO_HTTPS)?
                               TRUE:FALSE);
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    if(co) {
      struct Cookie *store=co;
      /* now loop through all cookies that matched */
      while(co) {
        if(co->value) {
          if(0 == count) {
            result = Curl_add_bufferf(req_buffer, "Cookie: ");
            if(result)
              break;
          }
          result = Curl_add_bufferf(req_buffer,
                                    "%s%s=%s", count?"; ":"",
                                    co->name, co->value);
          if(result)
            break;
          count++;
        }
        co = co->next; /* next cookie please */
      }
      Curl_cookie_freelist(store, FALSE); /* free the cookie list */
    }
    if(addcookies && !result) {
      if(!count)
        result = Curl_add_bufferf(req_buffer, "Cookie: ");
      if(!result) {
        result = Curl_add_bufferf(req_buffer, "%s%s", count?"; ":"",
                                  addcookies);
        count++;
      }
    }
    if(count && !result)
      result = Curl_add_buffer(req_buffer, "\r\n", 2);

    if(result)
      return result;
  }
#endif

  if(data->set.timecondition) {
    result = Curl_add_timecondition(data, req_buffer);
    if(result)
      return result;
  }

  result = Curl_add_custom_headers(conn, FALSE, req_buffer);
  if(result)
    return result;

  http->postdata = NULL;  /* nothing to post at this point */
  Curl_pgrsSetUploadSize(data, -1); /* upload size is unknown atm */

  /* If 'authdone' is FALSE, we must not set the write socket index to the
     Curl_transfer() call below, as we're not ready to actually upload any
     data yet. */

  switch(httpreq) {

  case HTTPREQ_POST_FORM:
    if(!http->sendit || conn->bits.authneg) {
      /* nothing to post! */
      result = Curl_add_bufferf(req_buffer, "Content-Length: 0\r\n\r\n");
      if(result)
        return result;

      result = Curl_add_buffer_send(req_buffer, conn,
                                    &data->info.request_size, 0, FIRSTSOCKET);
      if(result)
        failf(data, "Failed sending POST request");
      else
        /* setup variables for the upcoming transfer */
        Curl_setup_transfer(conn, FIRSTSOCKET, -1, TRUE, &http->readbytecount,
                            -1, NULL);
      break;
    }

    if(Curl_FormInit(&http->form, http->sendit)) {
      failf(data, "Internal HTTP POST error!");
      return CURLE_HTTP_POST_ERROR;
    }

    /* Get the currently set callback function pointer and store that in the
       form struct since we might want the actual user-provided callback later
       on. The conn->fread_func pointer itself will be changed for the
       multipart case to the function that returns a multipart formatted
       stream. */
    http->form.fread_func = conn->fread_func;

    /* Set the read function to read from the generated form data */
    conn->fread_func = (curl_read_callback)Curl_FormReader;
    conn->fread_in = &http->form;

    http->sending = HTTPSEND_BODY;

    if(!data->req.upload_chunky &&
       !Curl_checkheaders(conn, "Content-Length:")) {
      /* only add Content-Length if not uploading chunked */
      result = Curl_add_bufferf(req_buffer,
                                "Content-Length: %" CURL_FORMAT_CURL_OFF_T
                                "\r\n", http->postsize);
      if(result)
        return result;
    }

    result = expect100(data, conn, req_buffer);
    if(result)
      return result;

    {

      /* Get Content-Type: line from Curl_formpostheader.
       */
      char *contentType;
      size_t linelength=0;
      contentType = Curl_formpostheader((void *)&http->form,
                                        &linelength);
      if(!contentType) {
        failf(data, "Could not get Content-Type header line!");
        return CURLE_HTTP_POST_ERROR;
      }

      result = Curl_add_buffer(req_buffer, contentType, linelength);
      if(result)
        return result;
    }

    /* make the request end in a true CRLF */
    result = Curl_add_buffer(req_buffer, "\r\n", 2);
    if(result)
      return result;

    /* set upload size to the progress meter */
    Curl_pgrsSetUploadSize(data, http->postsize);

    /* fire away the whole request to the server */
    result = Curl_add_buffer_send(req_buffer, conn,
                                  &data->info.request_size, 0, FIRSTSOCKET);
    if(result)
      failf(data, "Failed sending POST request");
    else
      /* setup variables for the upcoming transfer */
      Curl_setup_transfer(conn, FIRSTSOCKET, -1, TRUE,
                          &http->readbytecount, FIRSTSOCKET,
                          &http->writebytecount);

    if(result) {
      Curl_formclean(&http->sendit); /* free that whole lot */
      return result;
    }

    /* convert the form data */
    result = Curl_convert_form(data, http->sendit);
    if(result) {
      Curl_formclean(&http->sendit); /* free that whole lot */
      return result;
    }

    break;

  case HTTPREQ_PUT: /* Let's PUT the data to the server! */

    if(conn->bits.authneg)
      postsize = 0;
    else
      postsize = data->state.infilesize;

    if((postsize != -1) && !data->req.upload_chunky &&
       !Curl_checkheaders(conn, "Content-Length:")) {
      /* only add Content-Length if not uploading chunked */
      result = Curl_add_bufferf(req_buffer,
                                "Content-Length: %" CURL_FORMAT_CURL_OFF_T
                                "\r\n", postsize);
      if(result)
        return result;
    }

    if(postsize != 0) {
      result = expect100(data, conn, req_buffer);
      if(result)
        return result;
    }

    result = Curl_add_buffer(req_buffer, "\r\n", 2); /* end of headers */
    if(result)
      return result;

    /* set the upload size to the progress meter */
    Curl_pgrsSetUploadSize(data, postsize);

    /* this sends the buffer and frees all the buffer resources */
    result = Curl_add_buffer_send(req_buffer, conn,
                                  &data->info.request_size, 0, FIRSTSOCKET);
    if(result)
      failf(data, "Failed sending PUT request");
    else
      /* prepare for transfer */
      Curl_setup_transfer(conn, FIRSTSOCKET, -1, TRUE,
                          &http->readbytecount, postsize?FIRSTSOCKET:-1,
                          postsize?&http->writebytecount:NULL);
    if(result)
      return result;
    break;

  case HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */

    if(conn->bits.authneg)
      postsize = 0;
    else {
      /* figure out the size of the postfields */
      postsize = (data->set.postfieldsize != -1)?
        data->set.postfieldsize:
        (data->set.postfields? (curl_off_t)strlen(data->set.postfields):-1);
    }

    /* We only set Content-Length and allow a custom Content-Length if
       we don't upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length) */
    if((postsize != -1) && !data->req.upload_chunky &&
       !Curl_checkheaders(conn, "Content-Length:")) {
      /* we allow replacing this header if not during auth negotiation,
         although it isn't very wise to actually set your own */
      result = Curl_add_bufferf(req_buffer,
                                "Content-Length: %" CURL_FORMAT_CURL_OFF_T
                                "\r\n", postsize);
      if(result)
        return result;
    }

    if(!Curl_checkheaders(conn, "Content-Type:")) {
      result = Curl_add_bufferf(req_buffer,
                                "Content-Type: application/"
                                "x-www-form-urlencoded\r\n");
      if(result)
        return result;
    }

    /* For really small posts we don't use Expect: headers at all, and for
       the somewhat bigger ones we allow the app to disable it. Just make
       sure that the expect100header is always set to the preferred value
       here. */
    ptr = Curl_checkheaders(conn, "Expect:");
    if(ptr) {
      data->state.expect100header =
        Curl_compareheader(ptr, "Expect:", "100-continue");
    }
    else if(postsize > TINY_INITIAL_POST_SIZE || postsize < 0) {
      result = expect100(data, conn, req_buffer);
      if(result)
        return result;
    }
    else
      data->state.expect100header = FALSE;

    if(data->set.postfields) {

      /* In HTTP2, we send request body in DATA frame regardless of
         its size. */
      if(conn->httpversion != 20 &&
         !data->state.expect100header &&
         (postsize < MAX_INITIAL_POST_SIZE))  {
        /* if we don't use expect: 100  AND
           postsize is less than MAX_INITIAL_POST_SIZE

           then append the post data to the HTTP request header. This limit
           is no magic limit but only set to prevent really huge POSTs to
           get the data duplicated with malloc() and family. */

        result = Curl_add_buffer(req_buffer, "\r\n", 2); /* end of headers! */
        if(result)
          return result;

        if(!data->req.upload_chunky) {
          /* We're not sending it 'chunked', append it to the request
             already now to reduce the number if send() calls */
          result = Curl_add_buffer(req_buffer, data->set.postfields,
                                   (size_t)postsize);
          included_body = postsize;
        }
        else {
          if(postsize) {
            /* Append the POST data chunky-style */
            result = Curl_add_bufferf(req_buffer, "%x\r\n", (int)postsize);
            if(!result) {
              result = Curl_add_buffer(req_buffer, data->set.postfields,
                                       (size_t)postsize);
              if(!result)
                result = Curl_add_buffer(req_buffer, "\r\n", 2);
              included_body = postsize + 2;
            }
          }
          if(!result)
            result = Curl_add_buffer(req_buffer, "\x30\x0d\x0a\x0d\x0a", 5);
          /* 0  CR  LF  CR  LF */
          included_body += 5;
        }
        if(result)
          return result;
        /* Make sure the progress information is accurate */
        Curl_pgrsSetUploadSize(data, postsize);
      }
      else {
        /* A huge POST coming up, do data separate from the request */
        http->postsize = postsize;
        http->postdata = data->set.postfields;

        http->sending = HTTPSEND_BODY;

        conn->fread_func = (curl_read_callback)readmoredata;
        conn->fread_in = (void *)conn;

        /* set the upload size to the progress meter */
        Curl_pgrsSetUploadSize(data, http->postsize);

        result = Curl_add_buffer(req_buffer, "\r\n", 2); /* end of headers! */
        if(result)
          return result;
      }
    }
    else {
      result = Curl_add_buffer(req_buffer, "\r\n", 2); /* end of headers! */
      if(result)
        return result;

      if(data->req.upload_chunky && conn->bits.authneg) {
        /* Chunky upload is selected and we're negotiating auth still, send
           end-of-data only */
        result = Curl_add_buffer(req_buffer,
                                 "\x30\x0d\x0a\x0d\x0a", 5);
        /* 0  CR  LF  CR  LF */
        if(result)
          return result;
      }

      else if(data->set.postfieldsize) {
        /* set the upload size to the progress meter */
        Curl_pgrsSetUploadSize(data, postsize?postsize:-1);

        /* set the pointer to mark that we will send the post body using the
           read callback, but only if we're not in authenticate
           negotiation  */
        if(!conn->bits.authneg) {
          http->postdata = (char *)&http->postdata;
          http->postsize = postsize;
        }
      }
    }
    /* issue the request */
    result = Curl_add_buffer_send(req_buffer, conn, &data->info.request_size,
                                  (size_t)included_body, FIRSTSOCKET);

    if(result)
      failf(data, "Failed sending HTTP POST request");
    else
      Curl_setup_transfer(conn, FIRSTSOCKET, -1, TRUE,
                          &http->readbytecount, http->postdata?FIRSTSOCKET:-1,
                          http->postdata?&http->writebytecount:NULL);
    break;

  default:
    result = Curl_add_buffer(req_buffer, "\r\n", 2);
    if(result)
      return result;

    /* issue the request */
    result = Curl_add_buffer_send(req_buffer, conn,
                                  &data->info.request_size, 0, FIRSTSOCKET);

    if(result)
      failf(data, "Failed sending HTTP request");
    else
      /* HTTP GET/HEAD download: */
      Curl_setup_transfer(conn, FIRSTSOCKET, -1, TRUE, &http->readbytecount,
                          http->postdata?FIRSTSOCKET:-1,
                          http->postdata?&http->writebytecount:NULL);
  }
  if(result)
    return result;

  if(http->writebytecount) {
    /* if a request-body has been sent off, we make sure this progress is noted
       properly */
    Curl_pgrsSetUploadCounter(data, http->writebytecount);
    if(Curl_pgrsUpdate(conn))
      result = CURLE_ABORTED_BY_CALLBACK;

    if(http->writebytecount >= postsize) {
      /* already sent the entire request body, mark the "upload" as
         complete */
      infof(data, "upload completely sent off: %" CURL_FORMAT_CURL_OFF_T
            " out of %" CURL_FORMAT_CURL_OFF_T " bytes\n",
            http->writebytecount, postsize);
      data->req.upload_done = TRUE;
      data->req.keepon &= ~KEEP_SEND; /* we're done writing */
      data->req.exp100 = EXP100_SEND_DATA; /* already sent */
    }
  }

  return result;
}