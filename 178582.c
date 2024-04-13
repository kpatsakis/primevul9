static CURLMcode multi_timeout(struct Curl_multi *multi,
                               long *timeout_ms)
{
  static struct timeval tv_zero = {0, 0};

  if(multi->timetree) {
    /* we have a tree of expire times */
    struct timeval now = Curl_tvnow();

    /* splay the lowest to the bottom */
    multi->timetree = Curl_splay(tv_zero, multi->timetree);

    if(Curl_splaycomparekeys(multi->timetree->key, now) > 0) {
      /* some time left before expiration */
      *timeout_ms = curlx_tvdiff(multi->timetree->key, now);
      if(!*timeout_ms)
        /*
         * Since we only provide millisecond resolution on the returned value
         * and the diff might be less than one millisecond here, we don't
         * return zero as that may cause short bursts of busyloops on fast
         * processors while the diff is still present but less than one
         * millisecond! instead we return 1 until the time is ripe.
         */
        *timeout_ms=1;
    }
    else
      /* 0 means immediately */
      *timeout_ms = 0;
  }
  else
    *timeout_ms = -1;

  return CURLM_OK;
}