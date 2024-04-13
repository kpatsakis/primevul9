output_auth_headers(struct connectdata *conn,
                    struct auth *authstatus,
                    const char *request,
                    const char *path,
                    bool proxy)
{
  const char *auth = NULL;
  CURLcode result = CURLE_OK;
#if defined(USE_SPNEGO) || !defined(CURL_DISABLE_VERBOSE_STRINGS)
  struct SessionHandle *data = conn->data;
#endif
#ifdef USE_SPNEGO
  struct negotiatedata *negdata = proxy?
    &data->state.proxyneg:&data->state.negotiate;
#endif

#ifdef CURL_DISABLE_CRYPTO_AUTH
  (void)request;
  (void)path;
#endif

#ifdef USE_SPNEGO
  negdata->state = GSS_AUTHNONE;
  if((authstatus->picked == CURLAUTH_NEGOTIATE) &&
     negdata->context && !GSS_ERROR(negdata->status)) {
    auth="Negotiate";
    result = Curl_output_negotiate(conn, proxy);
    if(result)
      return result;
    authstatus->done = TRUE;
    negdata->state = GSS_AUTHSENT;
  }
  else
#endif
#ifdef USE_NTLM
  if(authstatus->picked == CURLAUTH_NTLM) {
    auth="NTLM";
    result = Curl_output_ntlm(conn, proxy);
    if(result)
      return result;
  }
  else
#endif
#if defined(USE_NTLM) && defined(NTLM_WB_ENABLED)
  if(authstatus->picked == CURLAUTH_NTLM_WB) {
    auth="NTLM_WB";
    result = Curl_output_ntlm_wb(conn, proxy);
    if(result)
      return result;
  }
  else
#endif
#ifndef CURL_DISABLE_CRYPTO_AUTH
  if(authstatus->picked == CURLAUTH_DIGEST) {
    auth="Digest";
    result = Curl_output_digest(conn,
                                proxy,
                                (const unsigned char *)request,
                                (const unsigned char *)path);
    if(result)
      return result;
  }
  else
#endif
  if(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    if((proxy && conn->bits.proxy_user_passwd &&
        !Curl_checkProxyheaders(conn, "Proxy-authorization:")) ||
       (!proxy && conn->bits.user_passwd &&
        !Curl_checkheaders(conn, "Authorization:"))) {
      auth="Basic";
      result = http_output_basic(conn, proxy);
      if(result)
        return result;
    }
    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }

  if(auth) {
    infof(data, "%s auth using %s with user '%s'\n",
          proxy?"Proxy":"Server", auth,
          proxy?(conn->proxyuser?conn->proxyuser:""):
                (conn->user?conn->user:""));
    authstatus->multi = (!authstatus->done) ? TRUE : FALSE;
  }
  else
    authstatus->multi = FALSE;

  return CURLE_OK;
}