_gnutls_hash_reset (digest_hd_st * handle)
{
  if (handle->handle == NULL)
    {
      return;
    }

  handle->reset (handle->handle);
}