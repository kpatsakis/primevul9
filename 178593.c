struct Curl_multi *Curl_multi_handle(int hashsize, /* socket hash */
                                     int chashsize) /* connection hash */
{
  struct Curl_multi *multi = calloc(1, sizeof(struct Curl_multi));

  if(!multi)
    return NULL;

  multi->type = CURL_MULTI_HANDLE;

  if(Curl_mk_dnscache(&multi->hostcache))
    goto error;

  if(sh_init(&multi->sockhash, hashsize))
    goto error;

  if(Curl_conncache_init(&multi->conn_cache, chashsize))
    goto error;

  multi->msglist = Curl_llist_alloc(multi_freeamsg);
  if(!multi->msglist)
    goto error;

  multi->pending = Curl_llist_alloc(multi_freeamsg);
  if(!multi->pending)
    goto error;

  /* allocate a new easy handle to use when closing cached connections */
  multi->closure_handle = curl_easy_init();
  if(!multi->closure_handle)
    goto error;

  multi->closure_handle->multi = multi;
  multi->closure_handle->state.conn_cache = &multi->conn_cache;

  multi->max_pipeline_length = 5;

  /* -1 means it not set by user, use the default value */
  multi->maxconnects = -1;
  return multi;

  error:

  Curl_hash_destroy(&multi->sockhash);
  Curl_hash_destroy(&multi->hostcache);
  Curl_conncache_destroy(&multi->conn_cache);
  Curl_close(multi->closure_handle);
  multi->closure_handle = NULL;
  Curl_llist_destroy(multi->msglist, NULL);
  Curl_llist_destroy(multi->pending, NULL);

  free(multi);
  return NULL;
}