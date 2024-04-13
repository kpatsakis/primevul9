static int update_timer(struct Curl_multi *multi)
{
  long timeout_ms;

  if(!multi->timer_cb)
    return 0;
  if(multi_timeout(multi, &timeout_ms)) {
    return -1;
  }
  if(timeout_ms < 0) {
    static const struct timeval none={0, 0};
    if(Curl_splaycomparekeys(none, multi->timer_lastcall)) {
      multi->timer_lastcall = none;
      /* there's no timeout now but there was one previously, tell the app to
         disable it */
      return multi->timer_cb(multi, -1, multi->timer_userp);
    }
    return 0;
  }

  /* When multi_timeout() is done, multi->timetree points to the node with the
   * timeout we got the (relative) time-out time for. We can thus easily check
   * if this is the same (fixed) time as we got in a previous call and then
   * avoid calling the callback again. */
  if(Curl_splaycomparekeys(multi->timetree->key, multi->timer_lastcall) == 0)
    return 0;

  multi->timer_lastcall = multi->timetree->key;

  return multi->timer_cb(multi, timeout_ms, multi->timer_userp);
}