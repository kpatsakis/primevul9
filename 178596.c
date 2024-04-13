CURLMcode curl_multi_setopt(struct Curl_multi *multi,
                            CURLMoption option, ...)
{
  CURLMcode res = CURLM_OK;
  va_list param;

  if(!GOOD_MULTI_HANDLE(multi))
    return CURLM_BAD_HANDLE;

  va_start(param, option);

  switch(option) {
  case CURLMOPT_SOCKETFUNCTION:
    multi->socket_cb = va_arg(param, curl_socket_callback);
    break;
  case CURLMOPT_SOCKETDATA:
    multi->socket_userp = va_arg(param, void *);
    break;
  case CURLMOPT_PUSHFUNCTION:
    multi->push_cb = va_arg(param, curl_push_callback);
    break;
  case CURLMOPT_PUSHDATA:
    multi->push_userp = va_arg(param, void *);
    break;
  case CURLMOPT_PIPELINING:
    multi->pipelining = va_arg(param, long);
    break;
  case CURLMOPT_TIMERFUNCTION:
    multi->timer_cb = va_arg(param, curl_multi_timer_callback);
    break;
  case CURLMOPT_TIMERDATA:
    multi->timer_userp = va_arg(param, void *);
    break;
  case CURLMOPT_MAXCONNECTS:
    multi->maxconnects = va_arg(param, long);
    break;
  case CURLMOPT_MAX_HOST_CONNECTIONS:
    multi->max_host_connections = va_arg(param, long);
    break;
  case CURLMOPT_MAX_PIPELINE_LENGTH:
    multi->max_pipeline_length = va_arg(param, long);
    break;
  case CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
    multi->content_length_penalty_size = va_arg(param, long);
    break;
  case CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
    multi->chunk_length_penalty_size = va_arg(param, long);
    break;
  case CURLMOPT_PIPELINING_SITE_BL:
    res = Curl_pipeline_set_site_blacklist(va_arg(param, char **),
                                           &multi->pipelining_site_bl);
    break;
  case CURLMOPT_PIPELINING_SERVER_BL:
    res = Curl_pipeline_set_server_blacklist(va_arg(param, char **),
                                             &multi->pipelining_server_bl);
    break;
  case CURLMOPT_MAX_TOTAL_CONNECTIONS:
    multi->max_total_connections = va_arg(param, long);
    break;
  default:
    res = CURLM_UNKNOWN_OPTION;
    break;
  }
  va_end(param);
  return res;
}