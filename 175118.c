void Curl_flush_cookies(struct SessionHandle *data, int cleanup)
{
  if(data->set.str[STRING_COOKIEJAR]) {
    if(data->change.cookielist) {
      /* If there is a list of cookie files to read, do it first so that
         we have all the told files read before we write the new jar.
         Curl_cookie_loadfiles() LOCKS and UNLOCKS the share itself! */
      Curl_cookie_loadfiles(data);
    }

    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);

    /* if we have a destination file for all the cookies to get dumped to */
    if(cookie_output(data->cookies, data->set.str[STRING_COOKIEJAR]))
      infof(data, "WARNING: failed to save cookies in %s\n",
            data->set.str[STRING_COOKIEJAR]);
  }
  else {
    if(cleanup && data->change.cookielist) {
      /* since nothing is written, we can just free the list of cookie file
         names */
      curl_slist_free_all(data->change.cookielist); /* clean up list */
      data->change.cookielist = NULL;
    }
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  }

  if(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
    Curl_cookie_cleanup(data->cookies);
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}