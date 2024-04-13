static struct Curl_sh_entry *sh_addentry(struct curl_hash *sh,
                                         curl_socket_t s,
                                         struct Curl_easy *data)
{
  struct Curl_sh_entry *there = sh_getentry(sh, s);
  struct Curl_sh_entry *check;

  if(there)
    /* it is present, return fine */
    return there;

  /* not present, add it */
  check = calloc(1, sizeof(struct Curl_sh_entry));
  if(!check)
    return NULL; /* major failure */

  check->easy = data;
  check->socket = s;

  /* make/add new hash entry */
  if(!Curl_hash_add(sh, (char *)&s, sizeof(curl_socket_t), check)) {
    free(check);
    return NULL; /* major failure */
  }

  return check; /* things are good in sockhash land */
}