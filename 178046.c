static int call_disconnect_if_dead(struct connectdata *conn,
                                      void *param)
{
  struct Curl_easy* data = (struct Curl_easy*)param;
  disconnect_if_dead(conn, data);
  return 0; /* continue iteration */
}