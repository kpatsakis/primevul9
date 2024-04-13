void Curl_multi_dump(struct Curl_multi *multi)
{
  struct Curl_easy *data;
  int i;
  fprintf(stderr, "* Multi status: %d handles, %d alive\n",
          multi->num_easy, multi->num_alive);
  for(data=multi->easyp; data; data = data->next) {
    if(data->mstate < CURLM_STATE_COMPLETED) {
      /* only display handles that are not completed */
      fprintf(stderr, "handle %p, state %s, %d sockets\n",
              (void *)data,
              statename[data->mstate], data->numsocks);
      for(i=0; i < data->numsocks; i++) {
        curl_socket_t s = data->sockets[i];
        struct Curl_sh_entry *entry = sh_getentry(&multi->sockhash, s);

        fprintf(stderr, "%d ", (int)s);
        if(!entry) {
          fprintf(stderr, "INTERNAL CONFUSION\n");
          continue;
        }
        fprintf(stderr, "[%s %s] ",
                entry->action&CURL_POLL_IN?"RECVING":"",
                entry->action&CURL_POLL_OUT?"SENDING":"");
      }
      if(data->numsocks)
        fprintf(stderr, "\n");
    }
  }
}