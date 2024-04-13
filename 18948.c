router_get_extrainfo_hash(const char *s, char *digest)
{
  return router_get_hash_impl(s, strlen(s), digest, "extra-info",
                              "\nrouter-signature",'\n', DIGEST_SHA1);
}