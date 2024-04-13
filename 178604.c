CURLMcode curl_multi_add_handle(struct Curl_multi *multi,
                                struct Curl_easy *data)
{
  struct curl_llist *timeoutlist;

  /* First, make some basic checks that the CURLM handle is a good handle */
  if(!GOOD_MULTI_HANDLE(multi))
    return CURLM_BAD_HANDLE;

  /* Verify that we got a somewhat good easy handle too */
  if(!GOOD_EASY_HANDLE(data))
    return CURLM_BAD_EASY_HANDLE;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  if(data->multi)
    return CURLM_ADDED_ALREADY;

  /* Allocate and initialize timeout list for easy handle */
  timeoutlist = Curl_llist_alloc(multi_freetimeout);
  if(!timeoutlist)
    return CURLM_OUT_OF_MEMORY;

  /*
   * No failure allowed in this function beyond this point. And no
   * modification of easy nor multi handle allowed before this except for
   * potential multi's connection cache growing which won't be undone in this
   * function no matter what.
   */

  /* Make easy handle use timeout list initialized above */
  data->state.timeoutlist = timeoutlist;
  timeoutlist = NULL;

  /* set the easy handle */
  multistate(data, CURLM_STATE_INIT);

  if((data->set.global_dns_cache) &&
     (data->dns.hostcachetype != HCACHE_GLOBAL)) {
    /* global dns cache was requested but still isn't */
    struct curl_hash *global = Curl_global_host_cache_init();
    if(global) {
      /* only do this if the global cache init works */
      data->dns.hostcache = global;
      data->dns.hostcachetype = HCACHE_GLOBAL;
    }
  }
  /* for multi interface connections, we share DNS cache automatically if the
     easy handle's one is currently not set. */
  else if(!data->dns.hostcache ||
     (data->dns.hostcachetype == HCACHE_NONE)) {
    data->dns.hostcache = &multi->hostcache;
    data->dns.hostcachetype = HCACHE_MULTI;
  }

  /* Point to the multi's connection cache */
  data->state.conn_cache = &multi->conn_cache;

  /* This adds the new entry at the 'end' of the doubly-linked circular
     list of Curl_easy structs to try and maintain a FIFO queue so
     the pipelined requests are in order. */

  /* We add this new entry last in the list. */

  data->next = NULL; /* end of the line */
  if(multi->easyp) {
    struct Curl_easy *last = multi->easylp;
    last->next = data;
    data->prev = last;
    multi->easylp = data; /* the new last node */
  }
  else {
    /* first node, make prev NULL! */
    data->prev = NULL;
    multi->easylp = multi->easyp = data; /* both first and last */
  }

  /* make the Curl_easy refer back to this multi handle */
  data->multi = multi;

  /* Set the timeout for this handle to expire really soon so that it will
     be taken care of even when this handle is added in the midst of operation
     when only the curl_multi_socket() API is used. During that flow, only
     sockets that time-out or have actions will be dealt with. Since this
     handle has no action yet, we make sure it times out to get things to
     happen. */
  Curl_expire(data, 1);

  /* increase the node-counter */
  multi->num_easy++;

  /* increase the alive-counter */
  multi->num_alive++;

  /* A somewhat crude work-around for a little glitch in update_timer() that
     happens if the lastcall time is set to the same time when the handle is
     removed as when the next handle is added, as then the check in
     update_timer() that prevents calling the application multiple times with
     the same timer infor will not trigger and then the new handle's timeout
     will not be notified to the app.

     The work-around is thus simply to clear the 'lastcall' variable to force
     update_timer() to always trigger a callback to the app when a new easy
     handle is added */
  memset(&multi->timer_lastcall, 0, sizeof(multi->timer_lastcall));

  update_timer(multi);
  return CURLM_OK;
}