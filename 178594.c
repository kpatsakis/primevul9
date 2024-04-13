void Curl_multi_process_pending_handles(struct Curl_multi *multi)
{
  struct curl_llist_element *e = multi->pending->head;

  while(e) {
    struct Curl_easy *data = e->ptr;
    struct curl_llist_element *next = e->next;

    if(data->mstate == CURLM_STATE_CONNECT_PEND) {
      multistate(data, CURLM_STATE_CONNECT);

      /* Remove this node from the list */
      Curl_llist_remove(multi->pending, e, NULL);

      /* Make sure that the handle will be processed soonish. */
      Curl_expire_latest(data, 1);
    }

    e = next; /* operate on next handle */
  }
}