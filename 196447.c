static CURLcode http_perhapsrewind(struct connectdata *conn)
{
  struct SessionHandle *data = conn->data;
  struct HTTP *http = data->req.protop;
  curl_off_t bytessent;
  curl_off_t expectsend = -1; /* default is unknown */

  if(!http)
    /* If this is still NULL, we have not reach very far and we can safely
       skip this rewinding stuff */
    return CURLE_OK;

  switch(data->set.httpreq) {
  case HTTPREQ_GET:
  case HTTPREQ_HEAD:
    return CURLE_OK;
  default:
    break;
  }

  bytessent = http->writebytecount;

  if(conn->bits.authneg) {
    /* This is a state where we are known to be negotiating and we don't send
       any data then. */
    expectsend = 0;
  }
  else if(!conn->bits.protoconnstart) {
    /* HTTP CONNECT in progress: there is no body */
    expectsend = 0;
  }
  else {
    /* figure out how much data we are expected to send */
    switch(data->set.httpreq) {
    case HTTPREQ_POST:
      if(data->set.postfieldsize != -1)
        expectsend = data->set.postfieldsize;
      else if(data->set.postfields)
        expectsend = (curl_off_t)strlen(data->set.postfields);
      break;
    case HTTPREQ_PUT:
      if(data->state.infilesize != -1)
        expectsend = data->state.infilesize;
      break;
    case HTTPREQ_POST_FORM:
      expectsend = http->postsize;
      break;
    default:
      break;
    }
  }

  conn->bits.rewindaftersend = FALSE; /* default */

  if((expectsend == -1) || (expectsend > bytessent)) {
#if defined(USE_NTLM)
    /* There is still data left to send */
    if((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM) ||
       (data->state.authproxy.picked == CURLAUTH_NTLM_WB) ||
       (data->state.authhost.picked == CURLAUTH_NTLM_WB)) {
      if(((expectsend - bytessent) < 2000) ||
         (conn->ntlm.state != NTLMSTATE_NONE) ||
         (conn->proxyntlm.state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started *OR* there is just a little (<2K)
           data left to send, keep on sending. */

        /* rewind data when completely done sending! */
        if(!conn->bits.authneg) {
          conn->bits.rewindaftersend = TRUE;
          infof(data, "Rewind stream after send\n");
        }

        return CURLE_OK;
      }

      if(conn->bits.close)
        /* this is already marked to get closed */
        return CURLE_OK;

      infof(data, "NTLM send, close instead of sending %"
            CURL_FORMAT_CURL_OFF_T " bytes\n",
            (curl_off_t)(expectsend - bytessent));
    }
#endif

    /* This is not NTLM or many bytes left to send: close */
    connclose(conn, "Mid-auth HTTP and much data left to send");
    data->req.size = 0; /* don't download any more than 0 bytes */

    /* There still is data left to send, but this connection is marked for
       closure so we can safely do the rewind right now */
  }

  if(bytessent)
    /* we rewind now at once since if we already sent something */
    return Curl_readrewind(conn);

  return CURLE_OK;
}