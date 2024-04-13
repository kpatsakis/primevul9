CURLMcode curl_multi_cleanup(struct Curl_multi *multi)
{
  struct Curl_easy *data;
  struct Curl_easy *nextdata;

  if(GOOD_MULTI_HANDLE(multi)) {
    bool restore_pipe = FALSE;
    SIGPIPE_VARIABLE(pipe_st);

    multi->type = 0; /* not good anymore */

    /* Close all the connections in the connection cache */
    close_all_connections(multi);

    if(multi->closure_handle) {
      sigpipe_ignore(multi->closure_handle, &pipe_st);
      restore_pipe = TRUE;

      multi->closure_handle->dns.hostcache = &multi->hostcache;
      Curl_hostcache_clean(multi->closure_handle,
                           multi->closure_handle->dns.hostcache);

      Curl_close(multi->closure_handle);
    }

    Curl_hash_destroy(&multi->sockhash);
    Curl_conncache_destroy(&multi->conn_cache);
    Curl_llist_destroy(multi->msglist, NULL);
    Curl_llist_destroy(multi->pending, NULL);

    /* remove all easy handles */
    data = multi->easyp;
    while(data) {
      nextdata=data->next;
      if(data->dns.hostcachetype == HCACHE_MULTI) {
        /* clear out the usage of the shared DNS cache */
        Curl_hostcache_clean(data, data->dns.hostcache);
        data->dns.hostcache = NULL;
        data->dns.hostcachetype = HCACHE_NONE;
      }

      /* Clear the pointer to the connection cache */
      data->state.conn_cache = NULL;
      data->multi = NULL; /* clear the association */

      data = nextdata;
    }

    Curl_hash_destroy(&multi->hostcache);

    /* Free the blacklists by setting them to NULL */
    Curl_pipeline_set_site_blacklist(NULL, &multi->pipelining_site_bl);
    Curl_pipeline_set_server_blacklist(NULL, &multi->pipelining_server_bl);

    free(multi);
    if(restore_pipe)
      sigpipe_restore(&pipe_st);

    return CURLM_OK;
  }
  else
    return CURLM_BAD_HANDLE;
}