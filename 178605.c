static int multi_getsock(struct Curl_easy *data,
                         curl_socket_t *socks, /* points to numsocks number
                                                  of sockets */
                         int numsocks)
{
  /* If the pipe broke, or if there's no connection left for this easy handle,
     then we MUST bail out now with no bitmask set. The no connection case can
     happen when this is called from curl_multi_remove_handle() =>
     singlesocket() => multi_getsock().
  */
  if(data->state.pipe_broke || !data->easy_conn)
    return 0;

  if(data->mstate > CURLM_STATE_CONNECT &&
     data->mstate < CURLM_STATE_COMPLETED) {
    /* Set up ownership correctly */
    data->easy_conn->data = data;
  }

  switch(data->mstate) {
  default:
#if 0 /* switch back on these cases to get the compiler to check for all enums
         to be present */
  case CURLM_STATE_TOOFAST:  /* returns 0, so will not select. */
  case CURLM_STATE_COMPLETED:
  case CURLM_STATE_MSGSENT:
  case CURLM_STATE_INIT:
  case CURLM_STATE_CONNECT:
  case CURLM_STATE_WAITDO:
  case CURLM_STATE_DONE:
  case CURLM_STATE_LAST:
    /* this will get called with CURLM_STATE_COMPLETED when a handle is
       removed */
#endif
    return 0;

  case CURLM_STATE_WAITRESOLVE:
    return Curl_resolver_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_PROTOCONNECT:
  case CURLM_STATE_SENDPROTOCONNECT:
    return Curl_protocol_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_DO:
  case CURLM_STATE_DOING:
    return Curl_doing_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_WAITPROXYCONNECT:
    return waitproxyconnect_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_WAITCONNECT:
    return waitconnect_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_DO_MORE:
    return domore_getsock(data->easy_conn, socks, numsocks);

  case CURLM_STATE_DO_DONE: /* since is set after DO is completed, we switch
                               to waiting for the same as the *PERFORM
                               states */
  case CURLM_STATE_PERFORM:
  case CURLM_STATE_WAITPERFORM:
    return Curl_single_getsock(data->easy_conn, socks, numsocks);
  }

}