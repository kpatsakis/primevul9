_gnutls_get_hash_block_len (gnutls_digest_algorithm_t algo)
{
  switch (algo)
    {
    case GNUTLS_DIG_MD5:
    case GNUTLS_DIG_SHA1:
    case GNUTLS_DIG_RMD160:
    case GNUTLS_DIG_SHA256:
    case GNUTLS_DIG_SHA384:
    case GNUTLS_DIG_SHA512:
    case GNUTLS_DIG_SHA224:
      return 64;
    default:
      return 0;
    }
}