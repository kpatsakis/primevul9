_gnutls_hash (digest_hd_st * handle, const void *text, size_t textlen)
{
  if (textlen > 0)
    {
      handle->hash (handle->handle, text, textlen);
    }
  return 0;
}