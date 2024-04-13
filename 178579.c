static void mstate(struct Curl_easy *data, CURLMstate state
#ifdef DEBUGBUILD
                   , int lineno
#endif
)
{
  CURLMstate oldstate = data->mstate;
  static const init_multistate_func finit[CURLM_STATE_LAST] = {
    NULL,
    NULL,
    Curl_init_CONNECT, /* CONNECT */
    /* the rest is NULL too */
  };

#if defined(DEBUGBUILD) && defined(CURL_DISABLE_VERBOSE_STRINGS)
  (void) lineno;
#endif

  if(oldstate == state)
    /* don't bother when the new state is the same as the old state */
    return;

  data->mstate = state;

#if defined(DEBUGBUILD) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
  if(data->mstate >= CURLM_STATE_CONNECT_PEND &&
     data->mstate < CURLM_STATE_COMPLETED) {
    long connection_id = -5000;

    if(data->easy_conn)
      connection_id = data->easy_conn->connection_id;

    infof(data,
          "STATE: %s => %s handle %p; line %d (connection #%ld)\n",
          statename[oldstate], statename[data->mstate],
          (void *)data, lineno, connection_id);
  }
#endif

  if(state == CURLM_STATE_COMPLETED)
    /* changing to COMPLETED means there's one less easy handle 'alive' */
    data->multi->num_alive--;

  /* if this state has an init-function, run it */
  if(finit[state])
    finit[state](data);
}