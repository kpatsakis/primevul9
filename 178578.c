CURLMcode curl_multi_timeout(struct Curl_multi *multi,
                             long *timeout_ms)
{
  /* First, make some basic checks that the CURLM handle is a good handle */
  if(!GOOD_MULTI_HANDLE(multi))
    return CURLM_BAD_HANDLE;

  return multi_timeout(multi, timeout_ms);
}