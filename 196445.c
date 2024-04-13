static CURLcode expect100(struct SessionHandle *data,
                          struct connectdata *conn,
                          Curl_send_buffer *req_buffer)
{
  CURLcode result = CURLE_OK;
  const char *ptr;
  data->state.expect100header = FALSE; /* default to false unless it is set
                                          to TRUE below */
  if(use_http_1_1plus(data, conn) &&
     (conn->httpversion != 20)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    ptr = Curl_checkheaders(conn, "Expect:");
    if(ptr) {
      data->state.expect100header =
        Curl_compareheader(ptr, "Expect:", "100-continue");
    }
    else {
      result = Curl_add_bufferf(req_buffer,
                         "Expect: 100-continue\r\n");
      if(!result)
        data->state.expect100header = TRUE;
    }
  }
  return result;
}