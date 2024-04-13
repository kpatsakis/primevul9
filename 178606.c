CURLMcode curl_multi_socket(struct Curl_multi *multi, curl_socket_t s,
                            int *running_handles)
{
  CURLMcode result = multi_socket(multi, FALSE, s, 0, running_handles);
  if(CURLM_OK >= result)
    update_timer(multi);
  return result;
}