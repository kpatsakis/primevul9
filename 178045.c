find_oldest_idle_connection_in_bundle(struct Curl_easy *data,
                                      struct connectbundle *bundle)
{
  struct curl_llist_element *curr;
  timediff_t highscore = -1;
  timediff_t score;
  struct curltime now;
  struct connectdata *conn_candidate = NULL;
  struct connectdata *conn;

  (void)data;

  now = Curl_now();

  curr = bundle->conn_list.head;
  while(curr) {
    conn = curr->ptr;

    if(!conn->inuse) {
      /* Set higher score for the age passed since the connection was used */
      score = Curl_timediff(now, conn->now);

      if(score > highscore) {
        highscore = score;
        conn_candidate = conn;
      }
    }
    curr = curr->next;
  }

  return conn_candidate;
}