generate_hash_secret_salt(XML_Parser parser)
{
#if defined(__UINTPTR_TYPE__)
# define PARSER_CAST(p)  (__UINTPTR_TYPE__)(p)
#elif defined(_WIN64) && defined(_MSC_VER)
# define PARSER_CAST(p)  (unsigned __int64)(p)
#else
# define PARSER_CAST(p)  (p)
#endif

#ifdef __CloudABI__
  unsigned long entropy;
  (void)parser;
  (void)gather_time_entropy;
  arc4random_buf(&entropy, sizeof(entropy));
  return entropy;
#else
  /* Process ID is 0 bits entropy if attacker has local access
   * XML_Parser address is few bits of entropy if attacker has local access */
  const unsigned long entropy =
      gather_time_entropy() ^ getpid() ^ (unsigned long)PARSER_CAST(parser);

  /* Factors are 2^31-1 and 2^61-1 (Mersenne primes M31 and M61) */
  if (sizeof(unsigned long) == 4) {
    return entropy * 2147483647;
  } else {
    return entropy * (unsigned long)2305843009213693951;
  }
#endif
}