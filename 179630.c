_gnutls_hmac (digest_hd_st * handle, const void *text, size_t textlen)
{
  if (textlen > 0)
    {
      return handle->hash (handle->handle, text, textlen);
    }
  return 0;
}