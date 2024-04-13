CURLcode Curl_protocol_connecting(struct connectdata *conn,
                                  bool *done)
{
  CURLcode result = CURLE_OK;

  if(conn && conn->handler->connecting) {
    *done = FALSE;
    result = conn->handler->connecting(conn, done);
  }
  else
    *done = TRUE;

  return result;
}