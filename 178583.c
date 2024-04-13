void Curl_expire_latest(struct Curl_easy *data, long milli)
{
  struct timeval *expire = &data->state.expiretime;

  struct timeval set;

  set = Curl_tvnow();
  set.tv_sec += milli / 1000;
  set.tv_usec += (milli % 1000) * 1000;

  if(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  if(expire->tv_sec || expire->tv_usec) {
    /* This means that the struct is added as a node in the splay tree.
       Compare if the new time is earlier, and only remove-old/add-new if it
         is. */
    long diff = curlx_tvdiff(set, *expire);
    if(diff > 0)
      /* the new expire time was later than the top time, so just skip this */
      return;
  }

  /* Just add the timeout like normal */
  Curl_expire(data, milli);
}