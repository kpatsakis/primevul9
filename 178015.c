static int IsPipeliningPossible(const struct Curl_easy *handle,
                                const struct connectdata *conn)
{
  int avail = 0;

  /* If a HTTP protocol and pipelining is enabled */
  if((conn->handler->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->bits.protoconnstart || !conn->bits.close)) {

    if(Curl_pipeline_wanted(handle->multi, CURLPIPE_HTTP1) &&
       (handle->set.httpversion != CURL_HTTP_VERSION_1_0) &&
       (handle->set.httpreq == HTTPREQ_GET ||
        handle->set.httpreq == HTTPREQ_HEAD))
      /* didn't ask for HTTP/1.0 and a GET or HEAD */
      avail |= CURLPIPE_HTTP1;

    if(Curl_pipeline_wanted(handle->multi, CURLPIPE_MULTIPLEX) &&
       (handle->set.httpversion >= CURL_HTTP_VERSION_2))
      /* allows HTTP/2 */
      avail |= CURLPIPE_MULTIPLEX;
  }
  return avail;
}