_gnutls_hash_output (digest_hd_st * handle, void *digest)
{
  size_t maclen;

  maclen = _gnutls_hash_get_algo_len (handle->algorithm);

  if (digest != NULL)
    {
      handle->output (handle->handle, digest, maclen);
    }
}