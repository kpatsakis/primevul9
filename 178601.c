void Curl_multi_closed(struct connectdata *conn, curl_socket_t s)
{
  struct Curl_multi *multi = conn->data->multi;
  if(multi) {
    /* this is set if this connection is part of a handle that is added to
       a multi handle, and only then this is necessary */
    struct Curl_sh_entry *entry = sh_getentry(&multi->sockhash, s);

    if(entry) {
      if(multi->socket_cb)
        multi->socket_cb(conn->data, s, CURL_POLL_REMOVE,
                         multi->socket_userp,
                         entry->socketp);

      /* now remove it from the socket hash */
      sh_delentry(&multi->sockhash, s);
    }
  }
}