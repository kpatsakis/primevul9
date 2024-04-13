CURLcode Curl_http_input_auth(struct connectdata *conn, bool proxy,
                              const char *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
  struct SessionHandle *data = conn->data;

#ifdef USE_SPNEGO
  struct negotiatedata *negdata = proxy?
    &data->state.proxyneg:&data->state.negotiate;
#endif
  unsigned long *availp;
  struct auth *authp;

  if(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  else {
    availp = &data->info.httpauthavail;
    authp = &data->state.authhost;
  }

  /*
   * Here we check if we want the specific single authentication (using ==) and
   * if we do, we initiate usage of it.
   *
   * If the provided authentication is wanted as one out of several accepted
   * types (using &), we OR this authentication type to the authavail
   * variable.
   *
   * Note:
   *
   * ->picked is first set to the 'want' value (one or more bits) before the
   * request is sent, and then it is again set _after_ all response 401/407
   * headers have been received but then only to a single preferred method
   * (bit).
   *
   */

  while(*auth) {
#ifdef USE_SPNEGO
    if(checkprefix("Negotiate", auth)) {
      *availp |= CURLAUTH_NEGOTIATE;
      authp->avail |= CURLAUTH_NEGOTIATE;

      if(authp->picked == CURLAUTH_NEGOTIATE) {
        if(negdata->state == GSS_AUTHSENT || negdata->state == GSS_AUTHNONE) {
          CURLcode result = Curl_input_negotiate(conn, proxy, auth);
          if(!result) {
            DEBUGASSERT(!data->req.newurl);
            data->req.newurl = strdup(data->change.url);
            if(!data->req.newurl)
              return CURLE_OUT_OF_MEMORY;
            data->state.authproblem = FALSE;
            /* we received a GSS auth token and we dealt with it fine */
            negdata->state = GSS_AUTHRECV;
          }
          else
            data->state.authproblem = TRUE;
        }
      }
    }
    else
#endif
#ifdef USE_NTLM
      /* NTLM support requires the SSL crypto libs */
      if(checkprefix("NTLM", auth)) {
        *availp |= CURLAUTH_NTLM;
        authp->avail |= CURLAUTH_NTLM;
        if(authp->picked == CURLAUTH_NTLM ||
           authp->picked == CURLAUTH_NTLM_WB) {
          /* NTLM authentication is picked and activated */
          CURLcode result = Curl_input_ntlm(conn, proxy, auth);
          if(!result) {
            data->state.authproblem = FALSE;
#ifdef NTLM_WB_ENABLED
            if(authp->picked == CURLAUTH_NTLM_WB) {
              *availp &= ~CURLAUTH_NTLM;
              authp->avail &= ~CURLAUTH_NTLM;
              *availp |= CURLAUTH_NTLM_WB;
              authp->avail |= CURLAUTH_NTLM_WB;

              /* Get the challenge-message which will be passed to
               * ntlm_auth for generating the type 3 message later */
              while(*auth && ISSPACE(*auth))
                auth++;
              if(checkprefix("NTLM", auth)) {
                auth += strlen("NTLM");
                while(*auth && ISSPACE(*auth))
                  auth++;
                if(*auth)
                  if((conn->challenge_header = strdup(auth)) == NULL)
                    return CURLE_OUT_OF_MEMORY;
              }
            }
#endif
          }
          else {
            infof(data, "Authentication problem. Ignoring this.\n");
            data->state.authproblem = TRUE;
          }
        }
      }
      else
#endif
#ifndef CURL_DISABLE_CRYPTO_AUTH
        if(checkprefix("Digest", auth)) {
          if((authp->avail & CURLAUTH_DIGEST) != 0) {
            infof(data, "Ignoring duplicate digest auth header.\n");
          }
          else {
            CURLcode result;
            *availp |= CURLAUTH_DIGEST;
            authp->avail |= CURLAUTH_DIGEST;

            /* We call this function on input Digest headers even if Digest
             * authentication isn't activated yet, as we need to store the
             * incoming data from this header in case we are gonna use
             * Digest. */
            result = Curl_input_digest(conn, proxy, auth);
            if(result) {
              infof(data, "Authentication problem. Ignoring this.\n");
              data->state.authproblem = TRUE;
            }
          }
        }
        else
#endif
          if(checkprefix("Basic", auth)) {
            *availp |= CURLAUTH_BASIC;
            authp->avail |= CURLAUTH_BASIC;
            if(authp->picked == CURLAUTH_BASIC) {
              /* We asked for Basic authentication but got a 40X back
                 anyway, which basically means our name+password isn't
                 valid. */
              authp->avail = CURLAUTH_NONE;
              infof(data, "Authentication problem. Ignoring this.\n");
              data->state.authproblem = TRUE;
            }
          }

    /* there may be multiple methods on one line, so keep reading */
    while(*auth && *auth != ',') /* read up to the next comma */
      auth++;
    if(*auth == ',') /* if we're on a comma, skip it */
      auth++;
    while(*auth && ISSPACE(*auth))
      auth++;
  }
  return CURLE_OK;
}