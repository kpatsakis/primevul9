CURLMcode curl_multi_socket_all(struct Curl_multi *multi, int *running_handles)

{
  CURLMcode result = multi_socket(multi, TRUE, CURL_SOCKET_BAD, 0,
                                  running_handles);
  if(CURLM_OK >= result)
    update_timer(multi);
  return result;
}