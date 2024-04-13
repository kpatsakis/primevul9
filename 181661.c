CURLcode Curl_ntlm_core_mk_ntlmv2_hash(const char *user, size_t userlen,
                                       const char *domain, size_t domlen,
                                       unsigned char *ntlmhash,
                                       unsigned char *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned char *identity;
  CURLcode result = CURLE_OK;

  /* we do the length checks below separately to avoid integer overflow risk
     on extreme data lengths */
  if((userlen > SIZE_T_MAX/2) ||
     (domlen > SIZE_T_MAX/2) ||
     ((userlen + domlen) > SIZE_T_MAX/2))
    return CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = malloc(identity_len);

  if(!identity)
    return CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmac_md5(ntlmhash, 16, identity, curlx_uztoui(identity_len),
                         ntlmv2hash);

  free(identity);

  return result;
}