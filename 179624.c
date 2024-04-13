_gnutls_hmac_reset (digest_hd_st * handle)
{
  if (handle->handle == NULL)
    {
      return;
    }

  handle->reset (handle->handle);
}