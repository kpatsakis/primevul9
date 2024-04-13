_gnutls_hmac_output (digest_hd_st * handle, void *digest)
{
  size_t maclen;

  maclen = _gnutls_hmac_get_algo_len (handle->algorithm);

  if (digest != NULL)
    {
      handle->output (handle->handle, digest, maclen);
    }
}