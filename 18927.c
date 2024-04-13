router_get_networkstatus_v3_hash(const char *s, char *digest,
                                 digest_algorithm_t alg)
{
  return router_get_hash_impl(s, strlen(s), digest,
                              "network-status-version",
                              "\ndirectory-signature",
                              ' ', alg);
}