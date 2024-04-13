static bool isip(const char *domain)
{
  struct in_addr addr;
#ifdef ENABLE_IPV6
  struct in6_addr addr6;
#endif

  if(Curl_inet_pton(AF_INET, domain, &addr)
#ifdef ENABLE_IPV6
     || Curl_inet_pton(AF_INET6, domain, &addr6)
#endif
    ) {
    /* domain name given as IP address */
    return TRUE;
  }

  return FALSE;
}