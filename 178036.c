CURLcode Curl_protocol_doing(struct connectdata *conn, bool *done)
{
  CURLcode result = CURLE_OK;

  if(conn && conn->handler->doing) {
    *done = FALSE;
    result = conn->handler->doing(conn, done);
  }
  else
    *done = TRUE;

  return result;
}