static void sh_delentry(struct curl_hash *sh, curl_socket_t s)
{
  /* We remove the hash entry. This will end up in a call to
     sh_freeentry(). */
  Curl_hash_delete(sh, (char *)&s, sizeof(curl_socket_t));
}