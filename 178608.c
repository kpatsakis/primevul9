CURLMcode curl_multi_remove_handle(struct Curl_multi *multi,
                                   struct Curl_easy *data)
{
  struct Curl_easy *easy = data;
  bool premature;
  bool easy_owns_conn;
  struct curl_llist_element *e;

  /* First, make some basic checks that the CURLM handle is a good handle */
  if(!GOOD_MULTI_HANDLE(multi))
    return CURLM_BAD_HANDLE;

  /* Verify that we got a somewhat good easy handle too */
  if(!GOOD_EASY_HANDLE(data))
    return CURLM_BAD_EASY_HANDLE;

  /* Prevent users from trying to remove same easy handle more than once */
  if(!data->multi)
    return CURLM_OK; /* it is already removed so let's say it is fine! */

  premature = (data->mstate < CURLM_STATE_COMPLETED) ? TRUE : FALSE;
  easy_owns_conn = (data->easy_conn && (data->easy_conn->data == easy)) ?
    TRUE : FALSE;

  /* If the 'state' is not INIT or COMPLETED, we might need to do something
     nice to put the easy_handle in a good known state when this returns. */
  if(premature) {
    /* this handle is "alive" so we need to count down the total number of
       alive connections when this is removed */
    multi->num_alive--;

    /* When this handle gets removed, other handles may be able to get the
       connection */
    Curl_multi_process_pending_handles(multi);
  }

  if(data->easy_conn &&
     data->mstate > CURLM_STATE_DO &&
     data->mstate < CURLM_STATE_COMPLETED) {
    /* If the handle is in a pipeline and has started sending off its
       request but not received its response yet, we need to close
       connection. */
    connclose(data->easy_conn, "Removed with partial response");
    /* Set connection owner so that the DONE function closes it.  We can
       safely do this here since connection is killed. */
    data->easy_conn->data = easy;
    easy_owns_conn = TRUE;
  }

  /* The timer must be shut down before data->multi is set to NULL,
     else the timenode will remain in the splay tree after
     curl_easy_cleanup is called. */
  Curl_expire(data, 0);

  if(data->dns.hostcachetype == HCACHE_MULTI) {
    /* stop using the multi handle's DNS cache */
    data->dns.hostcache = NULL;
    data->dns.hostcachetype = HCACHE_NONE;
  }

  if(data->easy_conn) {

    /* we must call multi_done() here (if we still own the connection) so that
       we don't leave a half-baked one around */
    if(easy_owns_conn) {

      /* multi_done() clears the conn->data field to lose the association
         between the easy handle and the connection

         Note that this ignores the return code simply because there's
         nothing really useful to do with it anyway! */
      (void)multi_done(&data->easy_conn, data->result, premature);
    }
    else
      /* Clear connection pipelines, if multi_done above was not called */
      Curl_getoff_all_pipelines(data, data->easy_conn);
  }

  Curl_wildcard_dtor(&data->wildcard);

  /* destroy the timeout list that is held in the easy handle, do this *after*
     multi_done() as that may actually call Curl_expire that uses this */
  if(data->state.timeoutlist) {
    Curl_llist_destroy(data->state.timeoutlist, NULL);
    data->state.timeoutlist = NULL;
  }

  /* as this was using a shared connection cache we clear the pointer to that
     since we're not part of that multi handle anymore */
  data->state.conn_cache = NULL;

  /* change state without using multistate(), only to make singlesocket() do
     what we want */
  data->mstate = CURLM_STATE_COMPLETED;
  singlesocket(multi, easy); /* to let the application know what sockets that
                                vanish with this handle */

  /* Remove the association between the connection and the handle */
  if(data->easy_conn) {
    data->easy_conn->data = NULL;
    data->easy_conn = NULL;
  }

  data->multi = NULL; /* clear the association to this multi handle */

  /* make sure there's no pending message in the queue sent from this easy
     handle */

  for(e = multi->msglist->head; e; e = e->next) {
    struct Curl_message *msg = e->ptr;

    if(msg->extmsg.easy_handle == easy) {
      Curl_llist_remove(multi->msglist, e, NULL);
      /* there can only be one from this specific handle */
      break;
    }
  }

  /* make the previous node point to our next */
  if(data->prev)
    data->prev->next = data->next;
  else
    multi->easyp = data->next; /* point to first node */

  /* make our next point to our previous node */
  if(data->next)
    data->next->prev = data->prev;
  else
    multi->easylp = data->prev; /* point to last node */

  /* NOTE NOTE NOTE
     We do not touch the easy handle here! */
  multi->num_easy--; /* one less to care about now */

  update_timer(multi);
  return CURLM_OK;
}