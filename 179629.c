inline static int IS_SHA(gnutls_digest_algorithm_t algo)
{
  if (algo == GNUTLS_DIG_SHA1 || algo == GNUTLS_DIG_SHA224 ||
      algo == GNUTLS_DIG_SHA256 || algo == GNUTLS_DIG_SHA384 ||
      algo == GNUTLS_DIG_SHA512)
      return 1;
  return 0;
}