CURLcode Curl_http_readwrite_headers(struct SessionHandle *data,
                                       struct connectdata *conn,
                                       ssize_t *nread,
                                       bool *stop_reading)
{
  CURLcode result;
  struct SingleRequest *k = &data->req;

  /* header line within buffer loop */
  do {
    size_t rest_length;
    size_t full_length;
    int writetype;

    /* str_start is start of line within buf */
    k->str_start = k->str;

    /* data is in network encoding so use 0x0a instead of '\n' */
    k->end_ptr = memchr(k->str_start, 0x0a, *nread);

    if(!k->end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = header_append(data, k, *nread);
      if(result)
        return result;

      if(!k->headerline && (k->hbuflen>5)) {
        /* make a first check that this looks like a protocol header */
        if(!checkprotoprefix(data, conn, data->state.headerbuff)) {
          /* this is not the beginning of a protocol first header line */
          k->header = FALSE;
          k->badheader = HEADER_ALLBAD;
          break;
        }
      }

      break; /* read more and try again */
    }

    /* decrease the size of the remaining (supposed) header line */
    rest_length = (k->end_ptr - k->str)+1;
    *nread -= (ssize_t)rest_length;

    k->str = k->end_ptr + 1; /* move past new line */

    full_length = k->str - k->str_start;

    result = header_append(data, k, full_length);
    if(result)
      return result;

    k->end_ptr = k->hbufp;
    k->p = data->state.headerbuff;

    /****
     * We now have a FULL header line that p points to
     *****/

    if(!k->headerline) {
      /* the first read header */
      if((k->hbuflen>5) &&
         !checkprotoprefix(data, conn, data->state.headerbuff)) {
        /* this is not the beginning of a protocol first header line */
        k->header = FALSE;
        if(*nread)
          /* since there's more, this is a partial bad header */
          k->badheader = HEADER_PARTHEADER;
        else {
          /* this was all we read so it's all a bad header */
          k->badheader = HEADER_ALLBAD;
          *nread = (ssize_t)rest_length;
        }
        break;
      }
    }

    /* headers are in network encoding so
       use 0x0a and 0x0d instead of '\n' and '\r' */
    if((0x0a == *k->p) || (0x0d == *k->p)) {
      size_t headerlen;
      /* Zero-length header line means end of headers! */

#ifdef CURL_DOES_CONVERSIONS
      if(0x0d == *k->p) {
        *k->p = '\r'; /* replace with CR in host encoding */
        k->p++;       /* pass the CR byte */
      }
      if(0x0a == *k->p) {
        *k->p = '\n'; /* replace with LF in host encoding */
        k->p++;       /* pass the LF byte */
      }
#else
      if('\r' == *k->p)
        k->p++; /* pass the \r byte */
      if('\n' == *k->p)
        k->p++; /* pass the \n byte */
#endif /* CURL_DOES_CONVERSIONS */

      if(100 <= k->httpcode && 199 >= k->httpcode) {
        /*
         * We have made a HTTP PUT or POST and this is 1.1-lingo
         * that tells us that the server is OK with this and ready
         * to receive the data.
         * However, we'll get more headers now so we must get
         * back into the header-parsing state!
         */
        k->header = TRUE;
        k->headerline = 0; /* restart the header line counter */

        /* "A user agent MAY ignore unexpected 1xx status responses." */
        switch(k->httpcode) {
        case 100:
          /* if we did wait for this do enable write now! */
          if(k->exp100) {
            k->exp100 = EXP100_SEND_DATA;
            k->keepon |= KEEP_SEND;
          }
          break;
        case 101:
          /* Switching Protocols */
          if(k->upgr101 == UPGR101_REQUESTED) {
            infof(data, "Received 101\n");
            k->upgr101 = UPGR101_RECEIVED;

            /* switch to http2 now. The bytes after response headers
               are also processed here, otherwise they are lost. */
            result = Curl_http2_switched(conn, k->str, *nread);
            if(result)
              return result;
            *nread = 0;
          }
          break;
        default:
          break;
        }
      }
      else {
        k->header = FALSE; /* no more header to parse! */

        if((k->size == -1) && !k->chunk && !conn->bits.close &&
           (conn->httpversion == 11) &&
           !(conn->handler->protocol & CURLPROTO_RTSP) &&
           data->set.httpreq != HTTPREQ_HEAD) {
          /* On HTTP 1.1, when connection is not to get closed, but no
             Content-Length nor Content-Encoding chunked have been
             received, according to RFC2616 section 4.4 point 5, we
             assume that the server will close the connection to
             signal the end of the document. */
          infof(data, "no chunk, no close, no size. Assume close to "
                "signal end\n");
          connclose(conn, "HTTP: No end-of-message indicator");
        }
      }

      /*
       * When all the headers have been parsed, see if we should give
       * up and return an error.
       */
      if(http_should_fail(conn)) {
        failf (data, "The requested URL returned error: %d",
               k->httpcode);
        return CURLE_HTTP_RETURNED_ERROR;
      }

      /* now, only output this if the header AND body are requested:
       */
      writetype = CLIENTWRITE_HEADER;
      if(data->set.include_header)
        writetype |= CLIENTWRITE_BODY;

      headerlen = k->p - data->state.headerbuff;

      result = Curl_client_write(conn, writetype,
                                 data->state.headerbuff,
                                 headerlen);
      if(result)
        return result;

      data->info.header_size += (long)headerlen;
      data->req.headerbytecount += (long)headerlen;

      data->req.deductheadercount =
        (100 <= k->httpcode && 199 >= k->httpcode)?data->req.headerbytecount:0;

      if(!*stop_reading) {
        /* Curl_http_auth_act() checks what authentication methods
         * that are available and decides which one (if any) to
         * use. It will set 'newurl' if an auth method was picked. */
        result = Curl_http_auth_act(conn);

        if(result)
          return result;

        if(k->httpcode >= 300) {
          if((!conn->bits.authneg) && !conn->bits.close &&
             !conn->bits.rewindaftersend) {
            /*
             * General treatment of errors when about to send data. Including :
             * "417 Expectation Failed", while waiting for 100-continue.
             *
             * The check for close above is done simply because of something
             * else has already deemed the connection to get closed then
             * something else should've considered the big picture and we
             * avoid this check.
             *
             * rewindaftersend indicates that something has told libcurl to
             * continue sending even if it gets discarded
             */

            switch(data->set.httpreq) {
            case HTTPREQ_PUT:
            case HTTPREQ_POST:
            case HTTPREQ_POST_FORM:
              /* We got an error response. If this happened before the whole
               * request body has been sent we stop sending and mark the
               * connection for closure after we've read the entire response.
               */
              if(!k->upload_done) {
                infof(data, "HTTP error before end of send, stop sending\n");
                connclose(conn, "Stop sending data before everything sent");
                k->upload_done = TRUE;
                k->keepon &= ~KEEP_SEND; /* don't send */
                if(data->state.expect100header)
                  k->exp100 = EXP100_FAILED;
              }
              break;

            default: /* default label present to avoid compiler warnings */
              break;
            }
          }
        }

        if(conn->bits.rewindaftersend) {
          /* We rewind after a complete send, so thus we continue
             sending now */
          infof(data, "Keep sending data to get tossed away!\n");
          k->keepon |= KEEP_SEND;
        }
      }

      if(!k->header) {
        /*
         * really end-of-headers.
         *
         * If we requested a "no body", this is a good time to get
         * out and return home.
         */
        if(data->set.opt_no_body)
          *stop_reading = TRUE;
        else {
          /* If we know the expected size of this document, we set the
             maximum download size to the size of the expected
             document or else, we won't know when to stop reading!

             Note that we set the download maximum even if we read a
             "Connection: close" header, to make sure that
             "Content-Length: 0" still prevents us from attempting to
             read the (missing) response-body.
          */
          /* According to RFC2616 section 4.4, we MUST ignore
             Content-Length: headers if we are now receiving data
             using chunked Transfer-Encoding.
          */
          if(k->chunk)
            k->maxdownload = k->size = -1;
        }
        if(-1 != k->size) {
          /* We do this operation even if no_body is true, since this
             data might be retrieved later with curl_easy_getinfo()
             and its CURLINFO_CONTENT_LENGTH_DOWNLOAD option. */

          Curl_pgrsSetDownloadSize(data, k->size);
          k->maxdownload = k->size;
        }

        /* If max download size is *zero* (nothing) we already
           have nothing and can safely return ok now! */
        if(0 == k->maxdownload)
          *stop_reading = TRUE;

        if(*stop_reading) {
          /* we make sure that this socket isn't read more now */
          k->keepon &= ~KEEP_RECV;
        }

        if(data->set.verbose)
          Curl_debug(data, CURLINFO_HEADER_IN,
                     k->str_start, headerlen, conn);
        break;          /* exit header line loop */
      }

      /* We continue reading headers, so reset the line-based
         header parsing variables hbufp && hbuflen */
      k->hbufp = data->state.headerbuff;
      k->hbuflen = 0;
      continue;
    }

    /*
     * Checks for special headers coming up.
     */

    if(!k->headerline++) {
      /* This is the first header, it MUST be the error code line
         or else we consider this to be the body right away! */
      int httpversion_major;
      int rtspversion_major;
      int nc = 0;
#ifdef CURL_DOES_CONVERSIONS
#define HEADER1 scratch
#define SCRATCHSIZE 21
      CURLcode res;
      char scratch[SCRATCHSIZE+1]; /* "HTTP/major.minor 123" */
      /* We can't really convert this yet because we
         don't know if it's the 1st header line or the body.
         So we do a partial conversion into a scratch area,
         leaving the data at k->p as-is.
      */
      strncpy(&scratch[0], k->p, SCRATCHSIZE);
      scratch[SCRATCHSIZE] = 0; /* null terminate */
      res = Curl_convert_from_network(data,
                                      &scratch[0],
                                      SCRATCHSIZE);
      if(res)
        /* Curl_convert_from_network calls failf if unsuccessful */
        return res;
#else
#define HEADER1 k->p /* no conversion needed, just use k->p */
#endif /* CURL_DOES_CONVERSIONS */

      if(conn->handler->protocol & PROTO_FAMILY_HTTP) {
        /*
         * https://tools.ietf.org/html/rfc7230#section-3.1.2
         *
         * The reponse code is always a three-digit number in HTTP as the spec
         * says. We try to allow any number here, but we cannot make
         * guarantees on future behaviors since it isn't within the protocol.
         */
        nc = sscanf(HEADER1,
                    " HTTP/%d.%d %d",
                    &httpversion_major,
                    &conn->httpversion,
                    &k->httpcode);
        if(nc==3) {
          conn->httpversion += 10 * httpversion_major;

          if(k->upgr101 == UPGR101_RECEIVED) {
            /* supposedly upgraded to http2 now */
            if(conn->httpversion != 20)
              infof(data, "Lying server, not serving HTTP/2\n");
          }
        }
        else {
          /* this is the real world, not a Nirvana
             NCSA 1.5.x returns this crap when asked for HTTP/1.1
          */
          nc=sscanf(HEADER1, " HTTP %3d", &k->httpcode);
          conn->httpversion = 10;

          /* If user has set option HTTP200ALIASES,
             compare header line against list of aliases
          */
          if(!nc) {
            if(checkhttpprefix(data, k->p)) {
              nc = 1;
              k->httpcode = 200;
              conn->httpversion = 10;
            }
          }
        }
      }
      else if(conn->handler->protocol & CURLPROTO_RTSP) {
        nc = sscanf(HEADER1,
                    " RTSP/%d.%d %3d",
                    &rtspversion_major,
                    &conn->rtspversion,
                    &k->httpcode);
        if(nc==3) {
          conn->rtspversion += 10 * rtspversion_major;
          conn->httpversion = 11; /* For us, RTSP acts like HTTP 1.1 */
        }
        else {
          /* TODO: do we care about the other cases here? */
          nc = 0;
        }
      }

      if(nc) {
        data->info.httpcode = k->httpcode;

        data->info.httpversion = conn->httpversion;
        if(!data->state.httpversion ||
           data->state.httpversion > conn->httpversion)
          /* store the lowest server version we encounter */
          data->state.httpversion = conn->httpversion;

        /*
         * This code executes as part of processing the header.  As a
         * result, it's not totally clear how to interpret the
         * response code yet as that depends on what other headers may
         * be present.  401 and 407 may be errors, but may be OK
         * depending on how authentication is working.  Other codes
         * are definitely errors, so give up here.
         */
        if(data->set.http_fail_on_error && (k->httpcode >= 400) &&
           ((k->httpcode != 401) || !conn->bits.user_passwd) &&
           ((k->httpcode != 407) || !conn->bits.proxy_user_passwd) ) {

          if(data->state.resume_from &&
             (data->set.httpreq==HTTPREQ_GET) &&
             (k->httpcode == 416)) {
            /* "Requested Range Not Satisfiable", just proceed and
               pretend this is no error */
          }
          else {
            /* serious error, go home! */
            print_http_error(data);
            return CURLE_HTTP_RETURNED_ERROR;
          }
        }

        if(conn->httpversion == 10) {
          /* Default action for HTTP/1.0 must be to close, unless
             we get one of those fancy headers that tell us the
             server keeps it open for us! */
          infof(data, "HTTP 1.0, assume close after body\n");
          connclose(conn, "HTTP/1.0 close after body");
        }
        else if(conn->httpversion == 20 ||
                (k->upgr101 == UPGR101_REQUESTED && k->httpcode == 101)) {
          /* Don't enable pipelining for HTTP/2 or upgraded connection. For
             HTTP/2, we do not support multiplexing. In general, requests
             cannot be pipelined in upgraded connection, since it is now
             different protocol. */
          DEBUGF(infof(data,
                       "HTTP 2 or upgraded connection do not support "
                       "pipelining for now\n"));
        }
        else if(conn->httpversion >= 11 &&
                !conn->bits.close) {
          struct connectbundle *cb_ptr;

          /* If HTTP version is >= 1.1 and connection is persistent
             server supports pipelining. */
          DEBUGF(infof(data,
                       "HTTP 1.1 or later with persistent connection, "
                       "pipelining supported\n"));
          /* Activate pipelining if needed */
          cb_ptr = conn->bundle;
          if(cb_ptr) {
            if(!Curl_pipeline_site_blacklisted(data, conn))
              cb_ptr->server_supports_pipelining = TRUE;
          }
        }

        switch(k->httpcode) {
        case 204:
          /* (quote from RFC2616, section 10.2.5): The server has
           * fulfilled the request but does not need to return an
           * entity-body ... The 204 response MUST NOT include a
           * message-body, and thus is always terminated by the first
           * empty line after the header fields. */
          /* FALLTHROUGH */
        case 304:
          /* (quote from RFC2616, section 10.3.5): The 304 response
           * MUST NOT contain a message-body, and thus is always
           * terminated by the first empty line after the header
           * fields.  */
          if(data->set.timecondition)
            data->info.timecond = TRUE;
          k->size=0;
          k->maxdownload=0;
          k->ignorecl = TRUE; /* ignore Content-Length headers */
          break;
        default:
          /* nothing */
          break;
        }
      }
      else {
        k->header = FALSE;   /* this is not a header line */
        break;
      }
    }

    result = Curl_convert_from_network(data, k->p, strlen(k->p));
    /* Curl_convert_from_network calls failf if unsuccessful */
    if(result)
      return result;

    /* Check for Content-Length: header lines to get size */
    if(!k->ignorecl && !data->set.ignorecl &&
       checkprefix("Content-Length:", k->p)) {
      curl_off_t contentlength = curlx_strtoofft(k->p+15, NULL, 10);
      if(data->set.max_filesize &&
         contentlength > data->set.max_filesize) {
        failf(data, "Maximum file size exceeded");
        return CURLE_FILESIZE_EXCEEDED;
      }
      if(contentlength >= 0) {
        k->size = contentlength;
        k->maxdownload = k->size;
        /* we set the progress download size already at this point
           just to make it easier for apps/callbacks to extract this
           info as soon as possible */
        Curl_pgrsSetDownloadSize(data, k->size);
      }
      else {
        /* Negative Content-Length is really odd, and we know it
           happens for example when older Apache servers send large
           files */
        connclose(conn, "negative content-length");
        infof(data, "Negative content-length: %" CURL_FORMAT_CURL_OFF_T
              ", closing after transfer\n", contentlength);
      }
    }
    /* check for Content-Type: header lines to get the MIME-type */
    else if(checkprefix("Content-Type:", k->p)) {
      char *contenttype = Curl_copy_header_value(k->p);
      if(!contenttype)
        return CURLE_OUT_OF_MEMORY;
      if(!*contenttype)
        /* ignore empty data */
        free(contenttype);
      else {
        Curl_safefree(data->info.contenttype);
        data->info.contenttype = contenttype;
      }
    }
    else if(checkprefix("Server:", k->p)) {
      char *server_name = Curl_copy_header_value(k->p);

      /* Turn off pipelining if the server version is blacklisted */
      if(conn->bundle && conn->bundle->server_supports_pipelining) {
        if(Curl_pipeline_server_blacklisted(data, server_name))
          conn->bundle->server_supports_pipelining = FALSE;
      }
      free(server_name);
    }
    else if((conn->httpversion == 10) &&
            conn->bits.httpproxy &&
            Curl_compareheader(k->p,
                               "Proxy-Connection:", "keep-alive")) {
      /*
       * When a HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn, "Proxy-Connection keep-alive"); /* don't close */
      infof(data, "HTTP/1.0 proxy connection set to keep alive!\n");
    }
    else if((conn->httpversion == 11) &&
            conn->bits.httpproxy &&
            Curl_compareheader(k->p,
                               "Proxy-Connection:", "close")) {
      /*
       * We get a HTTP/1.1 response from a proxy and it says it'll
       * close down after this transfer.
       */
      connclose(conn, "Proxy-Connection: asked to close after done");
      infof(data, "HTTP/1.1 proxy connection set close!\n");
    }
    else if((conn->httpversion == 10) &&
            Curl_compareheader(k->p, "Connection:", "keep-alive")) {
      /*
       * A HTTP/1.0 reply with the 'Connection: keep-alive' line
       * tells us the connection will be kept alive for our
       * pleasure.  Default action for 1.0 is to close.
       *
       * [RFC2068, section 19.7.1] */
      connkeep(conn, "Connection keep-alive");
      infof(data, "HTTP/1.0 connection set to keep alive!\n");
    }
    else if(Curl_compareheader(k->p, "Connection:", "close")) {
      /*
       * [RFC 2616, section 8.1.2.1]
       * "Connection: close" is HTTP/1.1 language and means that
       * the connection will close when this request has been
       * served.
       */
      connclose(conn, "Connection: close used");
    }
    else if(checkprefix("Transfer-Encoding:", k->p)) {
      /* One or more encodings. We check for chunked and/or a compression
         algorithm. */
      /*
       * [RFC 2616, section 3.6.1] A 'chunked' transfer encoding
       * means that the server will send a series of "chunks". Each
       * chunk starts with line with info (including size of the
       * coming block) (terminated with CRLF), then a block of data
       * with the previously mentioned size. There can be any amount
       * of chunks, and a chunk-data set to zero signals the
       * end-of-chunks. */

      char *start;

      /* Find the first non-space letter */
      start = k->p + 18;

      for(;;) {
        /* skip whitespaces and commas */
        while(*start && (ISSPACE(*start) || (*start == ',')))
          start++;

        if(checkprefix("chunked", start)) {
          k->chunk = TRUE; /* chunks coming our way */

          /* init our chunky engine */
          Curl_httpchunk_init(conn);

          start += 7;
        }

        if(k->auto_decoding)
          /* TODO: we only support the first mentioned compression for now */
          break;

        if(checkprefix("identity", start)) {
          k->auto_decoding = IDENTITY;
          start += 8;
        }
        else if(checkprefix("deflate", start)) {
          k->auto_decoding = DEFLATE;
          start += 7;
        }
        else if(checkprefix("gzip", start)) {
          k->auto_decoding = GZIP;
          start += 4;
        }
        else if(checkprefix("x-gzip", start)) {
          k->auto_decoding = GZIP;
          start += 6;
        }
        else if(checkprefix("compress", start)) {
          k->auto_decoding = COMPRESS;
          start += 8;
        }
        else if(checkprefix("x-compress", start)) {
          k->auto_decoding = COMPRESS;
          start += 10;
        }
        else
          /* unknown! */
          break;

      }

    }
    else if(checkprefix("Content-Encoding:", k->p) &&
            (data->set.str[STRING_ENCODING] ||
             conn->httpversion == 20)) {
      /*
       * Process Content-Encoding. Look for the values: identity,
       * gzip, deflate, compress, x-gzip and x-compress. x-gzip and
       * x-compress are the same as gzip and compress. (Sec 3.5 RFC
       * 2616). zlib cannot handle compress.  However, errors are
       * handled further down when the response body is processed
       */
      char *start;

      /* Find the first non-space letter */
      start = k->p + 17;
      while(*start && ISSPACE(*start))
        start++;

      /* Record the content-encoding for later use */
      if(checkprefix("identity", start))
        k->auto_decoding = IDENTITY;
      else if(checkprefix("deflate", start))
        k->auto_decoding = DEFLATE;
      else if(checkprefix("gzip", start)
              || checkprefix("x-gzip", start))
        k->auto_decoding = GZIP;
      else if(checkprefix("compress", start)
              || checkprefix("x-compress", start))
        k->auto_decoding = COMPRESS;
    }
    else if(checkprefix("Content-Range:", k->p)) {
      /* Content-Range: bytes [num]-
         Content-Range: bytes: [num]-
         Content-Range: [num]-
         Content-Range: [asterisk]/[total]

         The second format was added since Sun's webserver
         JavaWebServer/1.1.1 obviously sends the header this way!
         The third added since some servers use that!
         The forth means the requested range was unsatisfied.
      */

      char *ptr = k->p + 14;

      /* Move forward until first digit or asterisk */
      while(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
        ptr++;

      /* if it truly stopped on a digit */
      if(ISDIGIT(*ptr)) {
        k->offset = curlx_strtoofft(ptr, NULL, 10);

        if(data->state.resume_from == k->offset)
          /* we asked for a resume and we got it */
          k->content_range = TRUE;
      }
      else
        data->state.resume_from = 0; /* get everything */
    }
#if !defined(CURL_DISABLE_COOKIES)
    else if(data->cookies &&
            checkprefix("Set-Cookie:", k->p)) {
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE,
                      CURL_LOCK_ACCESS_SINGLE);
      Curl_cookie_add(data,
                      data->cookies, TRUE, k->p+11,
                      /* If there is a custom-set Host: name, use it
                         here, or else use real peer host name. */
                      conn->allocptr.cookiehost?
                      conn->allocptr.cookiehost:conn->host.name,
                      data->state.path);
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
#endif
    else if(checkprefix("Last-Modified:", k->p) &&
            (data->set.timecondition || data->set.get_filetime) ) {
      time_t secs=time(NULL);
      k->timeofdoc = curl_getdate(k->p+strlen("Last-Modified:"),
                                  &secs);
      if(data->set.get_filetime)
        data->info.filetime = (long)k->timeofdoc;
    }
    else if((checkprefix("WWW-Authenticate:", k->p) &&
             (401 == k->httpcode)) ||
            (checkprefix("Proxy-authenticate:", k->p) &&
             (407 == k->httpcode))) {

      bool proxy = (k->httpcode == 407) ? TRUE : FALSE;
      char *auth = Curl_copy_header_value(k->p);
      if(!auth)
        return CURLE_OUT_OF_MEMORY;

      result = Curl_http_input_auth(conn, proxy, auth);

      free(auth);

      if(result)
        return result;
    }
    else if((k->httpcode >= 300 && k->httpcode < 400) &&
            checkprefix("Location:", k->p) &&
            !data->req.location) {
      /* this is the URL that the server advises us to use instead */
      char *location = Curl_copy_header_value(k->p);
      if(!location)
        return CURLE_OUT_OF_MEMORY;
      if(!*location)
        /* ignore empty data */
        free(location);
      else {
        data->req.location = location;

        if(data->set.http_follow_location) {
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = strdup(data->req.location); /* clone */
          if(!data->req.newurl)
            return CURLE_OUT_OF_MEMORY;

          /* some cases of POST and PUT etc needs to rewind the data
             stream at this point */
          result = http_perhapsrewind(conn);
          if(result)
            return result;
        }
      }
    }
    else if(conn->handler->protocol & CURLPROTO_RTSP) {
      result = Curl_rtsp_parseheader(conn, k->p);
      if(result)
        return result;
    }

    /*
     * End of header-checks. Write them to the client.
     */

    writetype = CLIENTWRITE_HEADER;
    if(data->set.include_header)
      writetype |= CLIENTWRITE_BODY;

    if(data->set.verbose)
      Curl_debug(data, CURLINFO_HEADER_IN,
                 k->p, (size_t)k->hbuflen, conn);

    result = Curl_client_write(conn, writetype, k->p, k->hbuflen);
    if(result)
      return result;

    data->info.header_size += (long)k->hbuflen;
    data->req.headerbytecount += (long)k->hbuflen;

    /* reset hbufp pointer && hbuflen */
    k->hbufp = data->state.headerbuff;
    k->hbuflen = 0;
  }
  while(!*stop_reading && *k->str); /* header line within buffer */

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */

  return CURLE_OK;
}