list_single_server_status(const routerinfo_t *desc, int is_live)
{
  char buf[MAX_NICKNAME_LEN+HEX_DIGEST_LEN+4]; /* !nickname=$hexdigest\0 */
  char *cp;
  const node_t *node;

  tor_assert(desc);

  cp = buf;
  if (!is_live) {
    *cp++ = '!';
  }
  node = node_get_by_id(desc->cache_info.identity_digest);
  if (node && node->is_valid) {
    strlcpy(cp, desc->nickname, sizeof(buf)-(cp-buf));
    cp += strlen(cp);
    *cp++ = '=';
  }
  *cp++ = '$';
  base16_encode(cp, HEX_DIGEST_LEN+1, desc->cache_info.identity_digest,
                DIGEST_LEN);
  return tor_strdup(buf);
}