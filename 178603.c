static struct Curl_sh_entry *sh_getentry(struct curl_hash *sh,
                                         curl_socket_t s)
{
  if(s != CURL_SOCKET_BAD)
    /* only look for proper sockets */
    return Curl_hash_pick(sh, (char *)&s, sizeof(curl_socket_t));
  return NULL;
}