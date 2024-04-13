CURLMcode Curl_multi_add_perform(struct Curl_multi *multi,
                                 struct Curl_easy *data,
                                 struct connectdata *conn)
{
  CURLMcode rc;

  rc = curl_multi_add_handle(multi, data);
  if(!rc) {
    struct SingleRequest *k = &data->req;

    /* pass in NULL for 'conn' here since we don't want to init the
       connection, only this transfer */
    Curl_init_do(data, NULL);

    /* take this handle to the perform state right away */
    multistate(data, CURLM_STATE_PERFORM);
    data->easy_conn = conn;
    k->keepon |= KEEP_RECV; /* setup to receive! */
  }
  return rc;
}