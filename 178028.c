static bool disconnect_if_dead(struct connectdata *conn,
                               struct Curl_easy *data)
{
  size_t pipeLen = conn->send_pipe.size + conn->recv_pipe.size;
  if(!pipeLen && !conn->inuse) {
    /* The check for a dead socket makes sense only if there are no
       handles in pipeline and the connection isn't already marked in
       use */
    bool dead;

    if(conn->handler->connection_check) {
      /* The protocol has a special method for checking the state of the
         connection. Use it to check if the connection is dead. */
      unsigned int state;

      state = conn->handler->connection_check(conn, CONNCHECK_ISDEAD);
      dead = (state & CONNRESULT_DEAD);
    }
    else {
      /* Use the general method for determining the death of a connection */
      dead = SocketIsDead(conn->sock[FIRSTSOCKET]);
    }

    if(dead) {
      conn->data = data;
      infof(data, "Connection %ld seems to be dead!\n", conn->connection_id);

      /* disconnect resources */
      Curl_disconnect(conn, /* dead_connection */TRUE);
      return TRUE;
    }
  }
  return FALSE;
}