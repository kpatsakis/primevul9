static CURLcode multi_do_more(struct connectdata *conn, int *complete)
{
  CURLcode result=CURLE_OK;

  *complete = 0;

  if(conn->handler->do_more)
    result = conn->handler->do_more(conn, complete);

  if(!result && (*complete == 1))
    /* do_complete must be called after the protocol-specific DO function */
    do_complete(conn);

  return result;
}