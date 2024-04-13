static CURLMcode add_next_timeout(struct timeval now,
                                  struct Curl_multi *multi,
                                  struct Curl_easy *d)
{
  struct timeval *tv = &d->state.expiretime;
  struct curl_llist *list = d->state.timeoutlist;
  struct curl_llist_element *e;

  /* move over the timeout list for this specific handle and remove all
     timeouts that are now passed tense and store the next pending
     timeout in *tv */
  for(e = list->head; e;) {
    struct curl_llist_element *n = e->next;
    long diff = curlx_tvdiff(*(struct timeval *)e->ptr, now);
    if(diff <= 0)
      /* remove outdated entry */
      Curl_llist_remove(list, e, NULL);
    else
      /* the list is sorted so get out on the first mismatch */
      break;
    e = n;
  }
  e = list->head;
  if(!e) {
    /* clear the expire times within the handles that we remove from the
       splay tree */
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }
  else {
    /* copy the first entry to 'tv' */
    memcpy(tv, e->ptr, sizeof(*tv));

    /* remove first entry from list */
    Curl_llist_remove(list, e, NULL);

    /* insert this node again into the splay */
    multi->timetree = Curl_splayinsert(*tv, multi->timetree,
                                       &d->state.timenode);
  }
  return CURLM_OK;
}