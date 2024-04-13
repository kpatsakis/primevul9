proxy_info_matches(const struct proxy_info* data,
                   const struct proxy_info* needle)
{
  if((data->proxytype == needle->proxytype) &&
     (data->port == needle->port) &&
     Curl_safe_strcasecompare(data->host.name, needle->host.name))
    return TRUE;

  return FALSE;
}