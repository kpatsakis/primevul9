CURLcode Curl_follow(struct Curl_easy *data,
                     char *newurl,    /* the Location: string */
                     followtype type) /* see transfer.h */
{
#ifdef CURL_DISABLE_HTTP
  (void)data;
  (void)newurl;
  (void)type;
  /* Location: following will not happen when HTTP is disabled */
  return CURLE_TOO_MANY_REDIRECTS;
#else

  /* Location: redirect */
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  CURLUcode uc;

  DEBUGASSERT(type != FOLLOW_NONE);

  if(type == FOLLOW_REDIR) {
    if((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    else {
      /* mark the next request as a followed location: */
      data->state.this_is_a_follow = TRUE;

      data->state.followlocation++; /* count location-followers */

      if(data->set.http_auto_referer) {
        CURLU *u;
        char *referer;

        /* We are asked to automatically set the previous URL as the referer
           when we get the next URL. We pick the ->url field, which may or may
           not be 100% correct */

        if(data->state.referer_alloc) {
          Curl_safefree(data->state.referer);
          data->state.referer_alloc = FALSE;
        }

        /* Make a copy of the URL without crenditals and fragment */
        u = curl_url();
        if(!u)
          return CURLE_OUT_OF_MEMORY;

        uc = curl_url_set(u, CURLUPART_URL, data->state.url, 0);
        if(!uc)
          uc = curl_url_set(u, CURLUPART_FRAGMENT, NULL, 0);
        if(!uc)
          uc = curl_url_set(u, CURLUPART_USER, NULL, 0);
        if(!uc)
          uc = curl_url_set(u, CURLUPART_PASSWORD, NULL, 0);
        if(!uc)
          uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

        curl_url_cleanup(u);

        if(uc || referer == NULL)
          return CURLE_OUT_OF_MEMORY;

        data->state.referer = referer;
        data->state.referer_alloc = TRUE; /* yes, free this later */
      }
    }
  }

  if((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, NULL, MAX_SCHEME_LEN))
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: don't allow a custom port number */
    disallowport = TRUE;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl,
                    (type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                    ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) );
  if(uc) {
    if(type != FOLLOW_FAKE)
      return Curl_uc_to_curlcode(uc);

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, just duplicate the field as-is */
    newurl = strdup(newurl);
    if(!newurl)
      return CURLE_OUT_OF_MEMORY;
  }
  else {

    uc = curl_url_get(data->state.uh, CURLUPART_URL, &newurl, 0);
    if(uc)
      return Curl_uc_to_curlcode(uc);
  }

  if(type == FOLLOW_FAKE) {
    /* we're only figuring out the new url if we would've followed locations
       but now we're done so we can get out! */
    data->info.wouldredirect = newurl;

    if(reachedmax) {
      failf(data, "Maximum (%ld) redirects followed", data->set.maxredirs);
      return CURLE_TOO_MANY_REDIRECTS;
    }
    return CURLE_OK;
  }

  if(disallowport)
    data->state.allow_port = FALSE;

  if(data->state.url_alloc)
    Curl_safefree(data->state.url);

  data->state.url = newurl;
  data->state.url_alloc = TRUE;

  infof(data, "Issue another request to this URL: '%s'\n", data->state.url);

  /*
   * We get here when the HTTP code is 300-399 (and 401). We need to perform
   * differently based on exactly what return code there was.
   *
   * News from 7.10.6: we can also get here on a 401 or 407, in case we act on
   * a HTTP (proxy-) authentication scheme other than Basic.
   */
  switch(data->info.httpcode) {
    /* 401 - Act on a WWW-Authenticate, we keep on moving and do the
       Authorization: XXXX header in the HTTP request code snippet */
    /* 407 - Act on a Proxy-Authenticate, we keep on moving and do the
       Proxy-Authorization: XXXX header in the HTTP request code snippet */
    /* 300 - Multiple Choices */
    /* 306 - Not used */
    /* 307 - Temporary Redirect */
  default:  /* for all above (and the unknown ones) */
    /* Some codes are explicitly mentioned since I've checked RFC2616 and they
     * seem to be OK to POST to.
     */
    break;
  case 301: /* Moved Permanently */
    /* (quote from RFC7231, section 6.4.2)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request.  If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    if((data->state.httpreq == HTTPREQ_POST
        || data->state.httpreq == HTTPREQ_POST_FORM
        || data->state.httpreq == HTTPREQ_POST_MIME)
       && !(data->set.keep_post & CURL_REDIR_POST_301)) {
      infof(data, "Switch from POST to GET\n");
      data->state.httpreq = HTTPREQ_GET;
    }
    break;
  case 302: /* Found */
    /* (quote from RFC7231, section 6.4.3)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request.  If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    if((data->state.httpreq == HTTPREQ_POST
        || data->state.httpreq == HTTPREQ_POST_FORM
        || data->state.httpreq == HTTPREQ_POST_MIME)
       && !(data->set.keep_post & CURL_REDIR_POST_302)) {
      infof(data, "Switch from POST to GET\n");
      data->state.httpreq = HTTPREQ_GET;
    }
    break;

  case 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     * https://github.com/curl/curl/issues/5237#issuecomment-614641049
     */
    if(data->state.httpreq != HTTPREQ_GET &&
       ((data->state.httpreq != HTTPREQ_POST &&
         data->state.httpreq != HTTPREQ_POST_FORM &&
         data->state.httpreq != HTTPREQ_POST_MIME) ||
        !(data->set.keep_post & CURL_REDIR_POST_303))) {
      data->state.httpreq = HTTPREQ_GET;
      data->set.upload = false;
      infof(data, "Switch to %s\n",
            data->set.opt_no_body?"HEAD":"GET");
    }
    break;
  case 304: /* Not Modified */
    /* 304 means we did a conditional request and it was "Not modified".
     * We shouldn't get any Location: header in this response!
     */
    break;
  case 305: /* Use Proxy */
    /* (quote from RFC2616, section 10.3.6):
     * "The requested resource MUST be accessed through the proxy given
     * by the Location field. The Location field gives the URI of the
     * proxy.  The recipient is expected to repeat this single request
     * via the proxy. 305 responses MUST only be generated by origin
     * servers."
     */
    break;
  }
  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  return CURLE_OK;
#endif /* CURL_DISABLE_HTTP */
}