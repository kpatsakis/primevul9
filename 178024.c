static void prune_dead_connections(struct Curl_easy *data)
{
  struct curltime now = Curl_now();
  time_t elapsed = Curl_timediff(now, data->state.conn_cache->last_cleanup);

  if(elapsed >= 1000L) {
    Curl_conncache_foreach(data, data->state.conn_cache, data,
                           call_disconnect_if_dead);
    data->state.conn_cache->last_cleanup = now;
  }
}