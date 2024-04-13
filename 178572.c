CURLMcode curl_multi_socket_action(struct Curl_multi *multi, curl_socket_t s,
                                   int ev_bitmask, int *running_handles)
{
  CURLMcode result = multi_socket(multi, FALSE, s,
                                  ev_bitmask, running_handles);
  if(CURLM_OK >= result)
    update_timer(multi);
  return result;
}